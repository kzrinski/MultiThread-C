#Creates a task_file with task# and random "cpu burst"
string=""
i="1"
rm task_file
while [ $i -lt 101 ]
do
string="$i $((1 + RANDOM %50))"
echo -e $string >> task_file
i=$[$i+1]
done
