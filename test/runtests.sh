#!/usr/bin/env bash
set -u

announce(){
    [[ -t 1 ]] && o="\e[1;33m$1\e[0m" || o=$1
    echo -e $o
}

warn(){
    [[ -t 1 ]] && o="\e[1;31m$1\e[0m" || o=$1
    echo -en $o
}

emphasize(){
   emphasize_n "$1" 
   echo
}

emphasize_n(){
    [[ -t 1 ]] && o="\e[1;39m$1\e[0m" || o=$1
    echo -ne $o
}

usage (){
cat << EOF
Test the final output of synder
OPTIONAL ARGUMENTS
  -h  print this help message
  -x  die on first failure 
  -m  test memory with valgrind
  -d  print full debugging info and link input file (g), database file (d),
      observed (o) and expected (e) files to the working directory.
  -v  verbose
  -o  redirect gdb output to this file (or tty)
  -a  archive all results
EOF
    exit 0
}

die_on_fail=0
debug=0
valgrind=0
verbose=0
archive=0
bigdata=0
gdb_out="none"
while getopts "hdxvma:o:" opt; do
    case $opt in
        h)
            usage ;;
        a)
            archive=$OPTARG
            mkdir -p $archive
            ;;
        d) 
            debug=1 ;;
        x)
            die_on_fail=1 ;;
        v)
            verbose=1 ;;
        o)
            gdb_out=$OPTARG
            ;;
        m)
            if [[ ! -z `type -P valgrind` ]]
            then
                valgrind=1
            else
                warn "Valgrind not found"   
            fi
            ;;
        ?)
            echo "Illegal arguments"
            exit 1
    esac 
done

synder=$PWD/synder

total_passed=0
total_failed=0
valgrind_checked=0
valgrind_exit_status=1
synder_db_exit_status=1
synder_exit_status=1
diff_exit_status=1


# A function to select which parts of the output should be compared
# Since flags are currently in flux, test only the first 7 columns
filter () {
    sort | cut -f1-8,11-13
}

filter_plus_one () {
    awk -v OFS="\t" '{$3++ ; $4++ ; $6++ ; $7++ ; print}' | filter
}

# This variable will be set for each set of test
# It specifies where the input files can be found
dir= arg= exp_ext= test_num=0
runtest(){
    base=$1
    msg=$2
    errmsg=${3:-0}
    out_base=${4:-0}

    echo -n "Testing $msg ... "

    fail=0

    [[ -z $exp_ext ]] && exp_ext=exp

    # initialize temporary files
    gff=input.gff
    map=synteny-map.tab
    gdbcmd=.gdb_cmd
    obs=observed-output
    exp=expected-output
    val=valgrind-log
    db=db
    tdb=$db/a_b.txt
    log=error-log
    run=run
    gdb=gdb
    syn=synmap.txt

    cp $dir/$base.gff $gff
    cp $dir/map.syn   $map

    # Default synder database building command
    db_cmd="$synder -d $map a b $db"

    # Query genome length file
    tgen=$dir/tgen.tab
    # Target genome length file
    qgen=$dir/qgen.tab

    # If the length files are given, add to db command
    if [[ -f $tgen ]]
    then
        db_cmd="$db_cmd $tgen"
    fi

    if [[ -f $tgen && -f $qgen ]]
    then
        db_cmd="$db_cmd $qgen"
    fi

    if [[ $out_base == 1 ]]
    then
        cat $dir/${base}-${exp_ext}.txt | filter_plus_one > $exp
    else
        cat $dir/${base}-${exp_ext}.txt | filter > $exp
    fi

    # Build database
    $db_cmd
    synder_db_exit_status=$?

    if [[ $synder_db_exit_status != 0 ]]
    then
        fail=1
        warn "database:"
    else

        synder_cmd=$synder

        [[ $out_base == 1 ]] && synder_cmd="$synder_cmd -b 0011 "
        synder_cmd="$synder_cmd -i $gff"
        synder_cmd="$synder_cmd -s $tdb"
        synder_cmd="$synder_cmd -c search"
        synder_cmd="$synder_cmd $arg"

        # command for loading into gdb
        echo "set args $synder_cmd"  >  $gdbcmd
        echo "source $PWD/.cmds.gdb" >> $gdbcmd
        echo "file $PWD/synder"      >> $gdbcmd
        if [[ $gdb_out != "none" ]]
        then
            echo "set logging off"                   >> $gdbcmd
            echo "set logging file $gdb_out"         >> $gdbcmd
            echo "set logging redirect on"           >> $gdbcmd
            echo "set logging on"                    >> $gdbcmd
            echo "gdb -tui --command $gdbcmd -d $PWD" > $gdb
            chmod 755 $gdb
        fi

        # Ensure all input files are readable
        for f in $gff $exp $map $tdb;
        do
            if [[ ! -r "$f" ]]
            then
                warn "input:"
                fail=1
            fi
        done

        $synder_cmd > $obs 2> $log
        synder_exit_status=$?

        if [[ $valgrind -eq 1 ]]
        then
            # append valgrind messages to any synder error messages
            valgrind --leak-check=full $synder_cmd > $obs 2> $val
            grep "ERROR SUMMARY: 0 errors" $val > /dev/null
            valgrind_exit_status=$?
            if [[ $valgrind_exit_status -ne 0 ]]
            then
                warn "valgrind:"
                fail=1
            fi
        fi

        $synder_cmd -D > /dev/null 2> $syn
        if [[ $? -ne 0 ]]
        then
            if [[ $valgrind_exit_status -eq 0 || $synder_exit_status -eq 0 ]]
            then
                warn "dump:"
                synder_exit_status=1
                fail=1
            fi
        fi

        if [[ $synder_exit_status != 0 ]]
        then
            warn "runtime:"
            fail=1
        fi

        filter < $obs > /tmp/z && mv /tmp/z $obs
        diff $exp $obs 2> /dev/null
        diff_exit_status=$?

        if [[ $fail -eq 0 && $diff_exit_status -ne 0 ]]
        then
            warn "logic:"
            fail=1
            [[ $errmsg == 0 ]] || (echo -e $errmsg | fmt)
            echo "======================================="
            emphasize_n "test directory"; echo ": `basename $dir`"
            emphasize_n "expected output"; echo ": (${base}-exp.txt)"
            cat $exp
            emphasize "observed output:"
            cat $obs
            emphasize_n "query gff"; echo ": (${base}.gff)"
            column -t $gff
            emphasize_n "synteny map"; echo ": (map.syn)"
            column -t $map
            if [[ $debug -eq 1 && $verbose -eq 1 ]]
            then
                echo "Debugging files:"
                echo " * g - input GFF file"
                echo " * o - observed output"
                echo " * e - expected output"
                echo " * d - database directory"
                echo " * v - valgrind log (empty on success)"
                echo " * l - error log (synder STDERR output)"
                echo " * s - synmap dump"
                echo " * c - gdb command"
                echo " * x - initialize gdb"
                echo " * r - run the command that failed"
                echo "Synder database command:"
                echo $db_cmd
                echo "Synder command:"
                echo $synder_cmd
            fi
            echo -e "---------------------------------------\n"
        fi
    fi

    if [[ -d $archive ]]
    then
        test_num=$(( test_num + 1 ))
        if [[ $fail -eq 1 ]]
        then
            arch="$archive/${test_num}_F_`basename $dir`"
        else
            arch="$archive/${test_num}_P_`basename $dir`"
        fi
        [[ -d $arch ]] && rm -rf $arch
        mkdir -p $arch
        mv $gff $map $gdbcmd $obs $exp $val $syn $db $log $run $gdb $arch 2> /dev/null
        echo $synder_cmd > $arch/$run
        chmod 755 $arch/$run
    fi

    # clear all temporary files
    rm -rf $gff $map $gdbcmd $obs $exp $val $db $log $gdb

    if [[ $fail -eq 0 ]]
    then
        echo "OK"
        total_passed=$(( $total_passed + 1 ))
    else
        warn "FAIL\n"
        total_failed=$(( $total_failed + 1 ))
        [[ $die_on_fail -eq 0 ]] || exit 1
    fi

    # Reset all values
    gff= map= cmd= obs= exp= val= db= tdb= log=

}

#---------------------------------------------------------------------
dir="$PWD/test/test-data/one-block"
announce "\nTesting with synteny map length == 1"
runtest hi     "query after of block"
runtest within "query within block"
runtest lo     "query before of block"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/two-block"
announce "\nTesting with synteny map length == 2"
runtest hi      "query downstream of all blocks"
runtest between "query between two blocks"
runtest lo      "query upstream of all blocks"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/multi-block"
announce "\nTesting with 5 adjacent blocks on the same strand"
runtest a "extreme left"
runtest b "inbetween two adjacent blocks"
runtest c "starts inbetween adjacent blocks"
runtest d "stops inbetween adjacent blocks"
runtest e "inbetween two adjacent blocks"
runtest f "starts before block 3, ends after block 3"
runtest g "starts in block 2, ends after block 3"
runtest h "starts before block 2, ends after block 3"
runtest i "starts in block 2, ends in block 2"
runtest j "extreme right"


#---------------------------------------------------------------------
dir="$PWD/test/test-data/simple-duplication"
announce "\nTest simple tandem duplication"
runtest between "query starts between the duplicated intervals"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/one-interval-inversion"
announce "\nTest when a single interval is inverted"
runtest between "query next to inverted interval"
runtest over    "query overlaps inverted interval"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/two-interval-inversion"
announce "\nTest when two interval are inverted"
runtest beside   "query next to inverted interval"
runtest within   "query between inverted intervals"
runtest spanning "query spans inverted intervals"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/tandem-transposition"
announce "\nTest tandem transposition"
runtest beside "query beside the transposed pair"
runtest within "query between the transposed pair"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/irregular-overlaps"
announce "\nTest target side internal overlaps"
runtest left "left side" "You are either 1) not sorting the by_stop vector
in Contig by Block stop positions, or 2) are snapping the search interval left
boundary to a Block that is nearest by start, but not be stop."
runtest right "right side"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/off-by-one"
announce "\nTest overlap edge cases"
runtest a "overlap of 1"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/inverted-extremes"
announce "\nExtreme value resulting from an inversion"
runtest extreme "between the query intervals, extreme SI"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/deletion"
announce "\nDeletion tests (adjacent bounds in target)"
runtest between "query is inbetween"

#---------------------------------------------------------------------
dir="$PWD/test/test-data/unassembled"
announce "\nMappings beyond the edges of target scaffold"
runtest lo "query is below scaffold"
runtest adj-lo "query is just below the scaffold"
runtest adj-hi "query is just above the scaffold"
runtest hi "query is above the scaffold"
runtest lo "test with 1-base" 0 1

#---------------------------------------------------------------------
announce "\nTest multi-chromosome cases when k=0"
arg=" -k 0 "
#  T   =====[---->
#        |
#  Q   =====   <->   =====
#                      |
#  T           <----]=====
dir="$PWD/test/test-data/interruptions/multi-chromosome"
runtest between "interuption between query intervals"
#  T   =====[-------------]=====
#        |                   |
#  Q   ===== <->   =====   =====
#                    |
#  T        ===[--]=====
#            |
#  Q        ===
dir="$PWD/test/test-data/interruptions/one-query-side"
runtest beside "query side"
# T             ===    ===
#                |      |
# Q    =====[--]===    ===[--]=====
#        |                      |
# T    =====   <-->           =====
dir="$PWD/test/test-data/interruptions/two-target-side"
runtest beside "target side"
arg=" -k 1 "
runtest beside "target side, k=1 (should be the same)"
# T    =====                      =====
#        |                          |
# Q    =====   ===== <--> =====   =====
#                |          |
# T            =====[----]=====
dir="$PWD/test/test-data/interruptions/two-query-side"
ark=" -k 0 "
runtest between "between two interior query-side intervals (k=0)"

#---------------------------------------------------------------------
args=' -k 4 '
exp_ext=
announce "\nConfirm two-scaffold systems are unaffected by k"
dir="$PWD/test/test-data/tandem-transposition"
runtest beside "query beside the transposed pair"
runtest within "query between the transposed pair"
dir="$PWD/test/test-data/simple-duplication"
runtest between "query starts between the duplicated intervals"

#---------------------------------------------------------------------
announce "\nTest multi-chromosome cases when k=2"
arg=" -k 2 "
exp_ext='exp-k2'
#  T   =====[-------------]=====
#        |                   |
#  Q   ===== <->   =====   =====
#                    |
#  T        ===[--]=====
#            |
#  Q        ===
dir="$PWD/test/test-data/interruptions/one-query-side"
runtest beside "query side"
#           [----------------]
# T             ===    ===
#                |      |
# Q    =====    ===    ===    =====
#        |                      |
# T    =====   <-->           =====
dir="$PWD/test/test-data/interruptions/two-target-side"
runtest beside "target side"
# T    =====[--------------------]=====
#        |                          |
# Q    =====   ===== <--> =====   =====
#                |          |
# T            =====[----]=====
dir="$PWD/test/test-data/interruptions/two-query-side"
runtest between "between two interior query-side intervals (k=2)"
# T    =====[------------------------------------]=====
#        |                                          |
# Q    =====   =====   ===== <--> =====   =====   =====
#                |       |          |       |
# T            =====[----|----------|----]=====
#                        |          |
#                      =====[----]=====
dir="$PWD/test/test-data/interruptions/nested"
arg=" -k 4 "
exp_ext="exp-k4"
runtest between "query nested two pairs of interrupting intervals (k=4)"
arg=" -k 3 "
exp_ext="exp-k3"
runtest between "query nested two pairs of interrupting intervals (k=3)"

arg=        # reset to default
exp_ext=exp # reset to default

#---------------------------------------------------------------------
args= exp_ext=
dir="$PWD/test/test-data/synmap-overlaps"
announce "\nsyntenic overlaps"
runtest simple "Between the weird"


#---------------------------------------------------------------------
dir="$PWD/test/test-data/big"
$synder -s "$dir/c.syn" -i "$dir/c.gff" -c search > /dev/null 2> /dev/null
if [[ $? -ne 0 ]]
then
    warn "\nFAILED STRESS TEST\n"
    total_failed=$(( total_failed + 1 ))
else
    total_passed=$(( total_passed + 1 ))
    announce "\nPASSED STRESS TEST\n"
fi

#---------------------------------------------------------------------
echo

total=$(( total_passed + total_failed))
emphasize "$total_passed tests successful out of $total"

if [[ $total_failed > 0 ]]
then
    warn "$total_failed tests failed\n"
    exit 1
else
    exit 0
fi
