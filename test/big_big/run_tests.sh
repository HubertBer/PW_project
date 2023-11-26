wn=$1
wpf=$2
womp=$3

mkdir output
mkdir time

./"$wn" < in > ./output/wn.out 2> ./time/wnt
echo "wn done!"

for i in {2..4}; do
    "$womp" "$i" < in > "./output/womp${i}.out"  2> "./time/womp${i}t"
    diff ./output/wn.out "./output/womp${i}.out" >> diffChecks.txt
    echo "${i} omp done!"
done

for i in {2..3}; do
    for j in {0..6}; do
        for k in {0..6}; do
            "$wpf" ${i} ${j} ${k} < in > "./output/wpf_${i}_${j}_${k}.out" 2> "./time/wpf_${i}_${j}_${k}t"
            diff ./output/wn.out "./output/wpf_${i}_${j}_${k}.out" >> diffChecks.txt
            echo "wpf_${i}_${j}_${k} done!"
        done
    done
done
