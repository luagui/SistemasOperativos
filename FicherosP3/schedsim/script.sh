
echo "Introudza el nombre del archivo a ejecutar:"
read filename

cd examples/


while [ ! -f $filename ]
do
	echo "No se encuentra el archio especificado."
	echo "Introudza el nombre del archivo a ejecutar:"
	read filename
done

cd ..

echo "Introudza el numero de cpus que desea simular:"
read numcpus

while [ $numcpus -gt  8 ]
do	
	echo "El numero maximo de cpus que se puede simular es 8."
	echo "Introudza el numero de cpus que desea simular:"
	read numcpus
done

if [ -d ./resultados ]
then
	rm -rf ./resultados
fi

mkdir ./resultados

availableScheluders="RR SJF FCFS PRIO"


for nameSched in $availableScheluders
do
	for ((cpus=1; cpus <= numcpus;cpus++))
	{
		./schedsim -i examples/$filename -n $cpus -s $nameSched

		for ((k=0; k < $cpus;k++))
		{	
			s=_CPU_
			t=_totalCpu_
			l=.log
			dest=$nameSched$t$cpus$s$k$l
			mv CPU_$k.log resultados/$dest
			cd ../gantt-gplot/
			./generate_gantt_chart ../schedsim/resultados/$dest
			cd ../schedsim/
		}
	}
done

exit 0
