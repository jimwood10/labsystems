n=0

while true
 do
  if test -r /ibm/fs1/primary/primaryTest
  then echo "Access OK $n"
  n=$((n+1))

  else
  echo "Access failed: $n"
  n=$((n+1))
  fi

sleep 2
done
