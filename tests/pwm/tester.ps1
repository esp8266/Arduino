# PWM tester

# Sigrok configuration
$sigrokcli = 'C:\Program Files (x86)\sigrok\sigrok-cli\sigrok-cli.exe'
$samplerate = "24mhz"
$samples = "120000"

# Open and close the COM port as fast as possible
function writeline([string]$text) {
	$com = new-Object System.IO.Ports.SerialPort COM6,115200,None,8,One
	$com.open()
	$com.WriteLine($text)
	$com.close()
}

# Probably smarter way to do this!
function stddev([int[]]$vals) {
	$sum = [float]0.0;
	foreach ($e in $vals) { $sum += $e }
	$avg = $sum / [float]$vals.length
	$sum = [float]0.0;
	foreach ($e in $vals) { $sum += ($e - $avg) * ($e - $avg) }
	$sum = $sum / [float]$vals.length
	$rho = [math]::sqrt($sum)
	return $rho
}

writeline("z") # Reset the chip
sleep 5
writeline("r {0}" -f 1000)


$freqs = 25000 # 1000, 5000, 10000, 20000, 30000, 40000

"PWMFREQ,VALUE,CYCLES,PERIODAVGNS,TIMEHIGHAVGNS,PERIODSTDDEVNS,TIMEHIGHSTDDEVNS"
foreach ($f in $freqs) {
	#";Testing frequency: {0}`n" -f [string]$f;
	writeline("f {0}" -f $f)
	sleep 1
	for ($i=0; $i -lt 1000; $i=$i+10) {
		#"Testing PWM: {0}`n" -f [string]$i;
		writeline("a {0} {1}" -f '8', $i)
		writeline("a {0} {1}" -f '9', [string](1000-[int]$i))
		sleep 1
		(((& $sigrokcli -d fx2lafw -C D5=p -O csv --samples $samples -c samplerate=$samplerate) | Select-Object -Skip 3 ) -replace ' ', '') > run.csv
		$csv = import-csv -path run.csv -delimiter ','
		$cnt = $csv.length
		# Skip any 0s to find first full edge
		$x = 0;
		$cyclecnt = 0;
		$pdttl = 0;
		$hittl = 0;
		$pl = New-Object Collections.Generic.List[float]
		$hl = New-Object Collections.Generic.List[float]
		while ( $csv[$x].logic -eq 0 ) { $x++ }
		for ($x=$x; $x -lt $cnt; $x++) {
			$h = $csv[$x].nanoseconds
			# Look for low
			for ($x=$x; ($csv[$x].logic -eq 1) -and ($x -lt $cnt); $x++ ) { }
			if ($x -lt $cnt) {
				# Found 1->0 transition, record it
				$l = $csv[$x].nanoseconds
				$timehigh = $l - $h
				# Now look for 0->1 so we can get period
				for ($x=$x; ($csv[$x].logic -eq 0) -and ($x -lt $cnt); $x++ ) { }
				if ($x -lt $cnt) {
					$period = $csv[$x].nanoseconds - $h
					$pdttl += $period
					$hittl += $timehigh
					$pl.Add($period)
					$hl.Add($timehigh)
					$cyclecnt++
					#"Period {0} TimeHigh {1}" -f [string]$period, [string]$timehigh
				}
			}
		}
		if ($cyclecnt -le 0) {
			"{0},{1},0,0,0,0,0" -f [string]$f, [string]$i
		} else {
			$pdavg = $pdttl / $cyclecnt
			$hiavg = $hittl / $cyclecnt
			$ps = stddev($pl)
			$hs = stddev($hl)
			"{0},{1},{2},{3},{4},{5},{6}" -f [string]$f, [string]$i, [string]$cyclecnt, [int]$pdavg, [int]$hiavg, [int]$ps, [int]$hs
		}
	}
}
