n=132
while [ $n -le 160 ]; do
    echo "Size $n"
    echo "Size $n" >> outProva.txt
    cnfgen tseitin $n 4 > tseitin-prova-d4-$n.cnf
    xmaplelcm -dip-pair-min=20 -cpu-lim=2500 tseitin-prova-d4-$n.cnf | grep "CPU"
    n=$((n+2))
done
