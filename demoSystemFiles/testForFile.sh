n=0

while true
 do
  if test -r /ibm/fs1/test
  then echo "Access OK $n"
  n=$((n+1))

  else
  echo "Access failed: $n"
  n=$((n+1))
  fi

sleep 2
done
