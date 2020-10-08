<?php
$filename = 'data.log';
$filenameDB = 'data.db';

if(!file_exists($filenameDB)) {file_put_contents($filenameDB, '');}

$db = new SQLite3($filenameDB, SQLITE3_OPEN_READWRITE);
$db->enableExceptions(true);

$db->query('CREATE TABLE IF NOT EXISTS sensor_values (timestamp TEXT, sensorIP TEXT, sensorID TEXT, data TEXT);');

if(isset($_POST) && !empty($_POST))
{
	$dataSet = array();
	$dataSet['datetime'] = date('YmdHis');
	$dataSet = array_merge($dataSet, $_POST);

	$data = json_encode($dataSet);

	$sensorID = '';
	if(isset($dataSet['id']) && !empty($dataSet['id']))
	{
		$sensorID = SQLite3::escapeString($dataSet['id']);
	}

	$db->query("INSERT INTO sensor_values (timestamp, sensorIP, sensorID, data) VALUES ('".$dataSet['datetime']."', '".$_SERVER['REMOTE_ADDR']."', '".$sensorID."', '".SQLite3::escapeString($data)."');");

	// save to log for now just to be save
	file_put_contents($filename, $data."\r\n".file_get_contents($filename));
	echo "ok";
}
else if(isset($_GET['show']))
{
	$num = 20;
	if(isset($_GET['num'])) { $num = (int)$_GET['num']; }
	if(isset($_GET['id'])) { $id = SQLite3::escapeString($_GET['id']); }

	$queryString = 'SELECT data FROM sensor_values';

	if(isset($id) && !empty($id))
	{
		$queryString .= ' WHERE sensorID = "'.$id.'"';
	}
	$queryString .= ' ORDER BY timestamp DESC LIMIT '.$num;

	$result = $db->query($queryString);

	$db_data = array();
	if($result->numColumns() > 0)
	{
		while ($res= $result->fetchArray(SQLITE3_ASSOC))
		{
			array_push($db_data, $res['data']);
		}
	}
	$result->finalize();
	$plotData = array_reverse($db_data);

	// parse file data and extract interesting information
	$temp = array();
	$relHum = array();
	$tvoc = array();
	$eCO2 = array();
	
	$chart1Min = 0;
	$chart1Max = 1;
	$chart2Min = 0;
	$chart2Max = 1;
	foreach($plotData as $arrayKey => $jsonValue)
	{
		$newValueArray = json_decode($jsonValue, true);
		
		$dtime = date_create_from_format("YmdHis", $newValueArray['datetime']);
		if(!$dtime)
		{
			// invalid date format
			continue;
		}
		$convertedDateTime = date_format($dtime, 'Y-m-d H:i:s');
		
		// this defines what can be plotted
		$eCO2[$arrayKey]['label'] = $convertedDateTime;
		$eCO2[$arrayKey]['y'] = $newValueArray['eCO2'];
		
		$tvoc[$arrayKey]['label'] = $convertedDateTime;
		$tvoc[$arrayKey]['y'] = $newValueArray['TVOC'];
		
		$chart1Min = min($chart1Min, (double)$newValueArray['eCO2'], (double)$newValueArray['TVOC']);
		$chart1Max = max($chart1Max, (double)$newValueArray['eCO2'], (double)$newValueArray['TVOC']);
		
		$temp[$arrayKey]['label'] = $convertedDateTime;
		$temp[$arrayKey]['y'] = $newValueArray['temp'];
		
		$relHum[$arrayKey]['label'] = $convertedDateTime;
		$relHum[$arrayKey]['y'] = $newValueArray['relHum'];
		
		$chart2Min = min($chart2Min, (double)$newValueArray['temp'], (double)$newValueArray['relHum']);
		$chart2Max = max($chart2Max, (double)$newValueArray['temp'], (double)$newValueArray['relHum']);
    }
?>
<!DOCTYPE HTML>
<html>
<head>
<script>
window.onload = function () {
 
var chart = new CanvasJS.Chart("chartContainer1", {
	title: {
		text: "Air Quality"
	},
	zoomEnabled: true,
	toolTip: {
		shared: true
	},
	legend: {
       horizontalAlign: "right", // "center" , "right"
       verticalAlign: "top",  // "top" , "bottom"
       fontSize: 15
     },
	axisX: {
        valueFormatString: "YYYYMMDDhhmmss",
		reversed: true
    },
	axisY: {
		title: "TVOC + eCO2",
		minimum: <?php echo $chart1Min; ?>,
		maximum: <?php echo 1.1*$chart1Max; ?>
	},
	axisY2: {
		title: "temp + relHum",
		minimum: <?php echo $chart2Min; ?>,
		maximum: <?php echo 1.1*$chart2Max; ?>
	},
	data: [
	{
		type: "line",
		name: "TVOC",
		showInLegend: true,
		axisYType: "primary",
      	legendText: "TVOC",
		dataPoints: <?php echo json_encode($tvoc, JSON_NUMERIC_CHECK); ?>
	},
	{
		type: "line",
		name: "eCO2",
		showInLegend: true,
		axisYType: "primary",
      	legendText: "eCO2",
		dataPoints: <?php echo json_encode($eCO2, JSON_NUMERIC_CHECK); ?>
	},
		{
		type: "line",
		name: "temp",
		showInLegend: true,
		axisYType: "secondary",
      	legendText: "temp",
		dataPoints: <?php echo json_encode($temp, JSON_NUMERIC_CHECK); ?>
	},
	{
		type: "line",
		name: "relHum",
		showInLegend: true,
		axisYType: "secondary",
      	legendText: "relHum",
		dataPoints: <?php echo json_encode($relHum, JSON_NUMERIC_CHECK); ?>
	}
	]
});
chart.render();
 
}
</script>
</head>
<body>
<div id="chartContainer1" style="height: 500px; width: 100%;"></div>
<script src="https://cdnjs.cloudflare.com/ajax/libs/canvasjs/1.7.0/canvasjs.min.js"></script>
</body>
</html>

<?php
}
?>