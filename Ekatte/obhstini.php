<?php header('Content-Type: text/html; charset=UTF-8'); ?>
<?php
require_once 'excel_reader2.php';
require_once 'db.php';



$Obshtina = new Spreadsheet_Excel_Reader();
$Obshtina->setUTFEncoder('iconv');
$Obshtina->setOutputEncoding('UTF-8');
$Obshtina->read("Ekatte/xls_ekatte/Ek_obst.xls");

for($i=0;$i<count($Obshtina->sheets);$i++) // Loop to get all sheets in a file.
{	
	if(count($Obshtina->sheets[$i][cells])>0) // checking sheet not empty
	{
		for($j=1;$j<=count($Obshtina->sheets[$i][cells]);$j++) // loop used to get each row of the sheet
		{	

			
			$Obshtina = $Obshtina->sheets[$i][cells][$j][1];
			$ekatte = $Obshtina->sheets[$i][cells][$j][2];
			$name = $Obshtina->sheets[$i][cells][$j][3];
			$category = $Obshtina->sheets[$i][cells][$j][4];
			$document = $Obshtina->sheets[$i][cells][$j][5];
			$abc = $Obshtina->sheets[$i][cells][$j][6];
			mysql_query("SET NAMES UTF8");
			$query1 = mysql_query("INSERT INTO Obshtina(Obshtina,ekatte,name,category,document,abc) values('".$Obshtina."','".$ekatte."','".$name."','".$category."','".$document."','".$abc."')");
			echo $Obshtina.' ';
		}	
	
	}
}


$Oblast = new Spreadsheet_Excel_Reader();
$Oblast->setUTFEncoder('iconv');
$Oblast->setOutputEncoding('UTF-8');
$Oblast->read("Ekatte/xls_ekatte/Ek_obl.xls");

for($i=0;$i<count($Oblast->sheets);$i++) // Loop to get all sheets in a file.
{	
	if(count($Oblast->sheets[$i][cells])>0) // checking sheet not empty
	{
		for($j=2;$j<=count($Oblast->sheets[$i][cells]);$j++) // loop used to get each row of the sheet
		{
			$Obl = $Oblast->sheets[$i][cells][$j][1];
			$ekatte = $Oblast->sheets[$i][cells][$j][2];
			$name = $Oblast->sheets[$i][cells][$j][3];
			$region = $Oblast->sheets[$i][cells][$j][4];
			$document = $Oblast->sheets[$i][cells][$j][3];
			$abc = $Oblast->sheets[$i][cells][$j][4];
			mysql_query("SET NAMES UTF8");
			$query1 = mysql_query("INSERT INTO Oblast(Oblast,ekatte,name,region,document,abc) values('".$Obl."','".$ekatte."','".$name."','".$region."','".$document."','".$abc."')");
		}
	}
}

?>
