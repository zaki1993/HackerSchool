<?php header('Content-Type: text/html; charset=UTF-8'); ?>
<?php
require_once 'excel_reader2.php';
require_once 'db.php';


$Kmetstvo = new Spreadsheet_Excel_Reader();
$Kmetstvo->setUTFEncoder('iconv');
$Kmetstvo->setOutputEncoding('UTF-8');
$Kmetstvo->read("Ekatte/xls_ekatte/Ek_kmet.xls");

for($i=0;$i<count($Kmetstvo->sheets);$i++) // Loop to get all sheets in a file.
{	
	if(count($Kmetstvo->sheets[$i][cells])>0) // checking sheet not empty
	{
		for($j=2;$j<=count($Kmetstvo->sheets[$i][cells]);$j++) // loop used to get each row of the sheet
		{
			$kmet = $Kmetstvo->sheets[$i][cells][$j][1];
			$name = $Kmetstvo->sheets[$i][cells][$j][2];
			$center = $Kmetstvo->sheets[$i][cells][$j][3];
			$document = $Kmetstvo->sheets[$i][cells][$j][4];
			$category = $Kmetstvo->sheets[$i][cells][$j][5];
			mysql_query("SET NAMES UTF8");
			mysql_query("INSERT INTO Kmetstvo(kmetstvo,name,center,document,category) values('".$kmet."','".$name."','".$center."','".$document."','".$category."')");
		}
		
	}
}
?>


