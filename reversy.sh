#!/bin/bash

reversy=./reversy
tmp_in=/tmp/rev$$.in
tmp_out=/tmp/rev$$.out
log=reversy$$.log
color='+'
srch_depth=5
i=1
#trap "rm -f $tmp_in $tmp_out 2>/dev/null" INT TERM HUP EXIT
echo "[$i] Making turn by color $color" 
$reversy -d $srch_depth -l $log -c "$color" >$tmp_out
while true; do
	((i=i+1))
	if [ "$color" = "+" ]; then
		color="*"
	else
		color="+"
	fi
	mv $tmp_out $tmp_in
	echo "[$i] Making turn by color $color" 
	$reversy -i $tmp_in -d $srch_depth -l $log -c "$color" >$tmp_out
	e_code=$?
	if [ "$e_code" -eq "255" ]; then
		break
	else
		if [ "$e_code" -eq "254" ]; then
			echo "[$i] Color $color is skipping a turn"
		else
			if [ "$e_code" -ne "0" ]; then
				echo -e "Error occurred.\n$reversy exit code is $e_code"
				exit $e_code
 			fi
		fi
	fi
done
echo -e "Game is over\nFinal position:"
cat $tmp_out
cat $tmp_out >>$log
rm -f $tmp_in $tmp_out 2>/dev/null
