<?php header('Content-Type: text/html; charset=UTF-8'); ?>
<?php
require_once 'excel_reader2.php';
require_once 'db.php';


$region = new Spreadsheet_Excel_Reader();
$region->setUTFEncoder('iconv');
$region->setOutputEncoding('UTF-8');
$region->read("Ekatte/xls_ekatte/Ek_reg2.xls");

for($i=0;$i<count($region->sheets);$i++) // Loop to get all sheets in a file.
{	
	if(count($region->sheets[$i][cells])>0) // checking sheet not empty
	{
		for($j=2;$j<=count($region->sheets[$i][cells]);$j++) // loop used to get each row of the sheet
		{
			$reg = $region->sheets[$i][cells][$j][1];
			$name = $region->sheets[$i][cells][$j][2];
			$document = $region->sheets[$i][cells][$j][3];
			$abc = $region->sheets[$i][cells][$j][4];
			mysql_query("SET NAMES UTF8");
			$query1 = mysql_query("INSERT INTO Region(Region,Name,Document,abc) values('".$reg."','".$name."','".$document."','".$abc."')");
		}
	}
}

?>
