extract_time () {
    file=$1
    t=`grep "total" $file | awk '{print $2}' #print 2th column`
    echo $t
}

extract_conflicts () {
    file=$1
    t=`grep "conflicts" $file | tail -1 | awk '{print $3}' #print 4th column`
    echo $t
}

extract_decisions () {
    file=$1
    t=`grep "c decisions" $file | tail -1 | awk '{print $3}' #print 4th column`
    echo $t
}

extract_result () {
    file=$1
    t=`egrep "SATISF|UNKNO" $file | awk '{print $2}' #print 4th column`
    echo $t
}


for file in $1/*.res.KISSAT*; do
    time=$(extract_time $file)
    decs=$(extract_decisions $file)
    confs=$(extract_conflicts $file)
    stat=$(extract_result $file)
    name=`basename $file`
    echo "$name;$stat;$time;$confs;$decs"
done
