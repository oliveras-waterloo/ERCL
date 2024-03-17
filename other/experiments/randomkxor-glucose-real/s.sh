#######################################################

 echo "cnfgen randkxor 3 n n -T or 3"
n=400
while [ $n -le 600 ]; do
    echo $n
    echo "XMAPLELCM;GLUCOSE"
    for i in {1..5}; do
	cnfgen randkxor 3 $n $n -T or 3 > s.cnf	

	/usr/bin/time -o "time.txt" -f "%e" --quiet xmaplelcm -cpu-lim=300 s.cnf > xmaple.txt
	maple=`cat time.txt`	
	rm time.txt

	/usr/bin/time -o "time.txt" -f "%e" --quiet glucoser s.cnf >& glucoser.txt
	glucoser=`cat time.txt`	
	rm time.txt

	echo -n $maple
	echo -n ";"
	echo $glucoser
    done
    n=$((n+10))
done

#######################################################


echo "cnfgen randkxor 3 n n -T xor 2"
n=50
while [ $n -le 300 ]; do
    echo $n
    echo "XMAPLELCM;GLUCOSE"
    for i in {1..5}; do
	cnfgen randkxor 3 $n $n -T xor 2 > s.cnf	

	/usr/bin/time -o "time.txt" -f "%e" --quiet xmaplelcm -cpu-lim=300 s.cnf > xmaple.txt
	maple=`cat time.txt`	
	rm time.txt

	/usr/bin/time -o "time.txt" -f "%e" --quiet glucoser s.cnf >& glucoser.txt
	glucoser=`cat time.txt`	
	rm time.txt

	echo -n $maple
	echo -n ";"
	echo $glucoser
    done
    n=$((n+10))
done

#######################################################

echo "cnfgen randkxor 3 n n -T or 2"
n=150
while [ $n -le 450 ]; do
    echo $n
    echo "XMAPLELCM;GLUCOSE"
    for i in {1..5}; do
	cnfgen randkxor 3 $n $n -T or 2 > s.cnf	

	/usr/bin/time -o "time.txt" -f "%e" --quiet xmaplelcm -cpu-lim=300 s.cnf > xmaple.txt
	maple=`cat time.txt`	
	rm time.txt

	/usr/bin/time -o "time.txt" -f "%e" --quiet glucoser s.cnf >& glucoser.txt
	glucoser=`cat time.txt`	
	rm time.txt

	echo -n $maple
	echo -n ";"
	echo $glucoser
    done
    n=$((n+10))
done

#######################################################

echo "cnfgen randkxor 3 n n -T xor 3"
n=20
while [ $n -le 200 ]; do
    echo $n
    echo "XMAPLELCM;GLUCOSE"
    for i in {1..5}; do
	cnfgen randkxor 3 $n $n -T xor 3 > s.cnf	

	/usr/bin/time -o "time.txt" -f "%e" --quiet xmaplelcm -cpu-lim=300 s.cnf > xmaple.txt
	maple=`cat time.txt`	
	rm time.txt

	/usr/bin/time -o "time.txt" -f "%e" --quiet glucoser  s.cnf >& glucoser.txt
	glucoser=`cat time.txt`	
	rm time.txt

	echo -n $maple
	echo -n ";"
	echo $glucoser
    done
    n=$((n+10))
done

