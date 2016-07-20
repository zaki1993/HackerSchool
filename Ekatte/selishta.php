<?php header('Content-Type: text/html; charset=UTF-8'); ?>
<?php
require_once 'excel_reader2.php';
require_once 'db.php';

//Selishte

$selishte = new Spreadsheet_Excel_Reader();
$selishte->setUTFEncoder('iconv');
$selishte->setOutputEncoding('UTF-8');
$selishte->read("Ekatte/xls_ekatte/Ek_atte.xls");

for($i=0;$i<count($selishte->sheets);$i++) // Loop to get all sheets in a file.
{	
	if(count($selishte->sheets[$i][cells])>0) // checking sheet not empty
	{
		for($j=3;$j<=count($selishte->sheets[$i][cells]);$j++) // loop used to get each row of the sheet
		{
			$ekatte = $selishte->sheets[$i][cells][$j][1];
			$tvm = $selishte->sheets[$i][cells][$j][2];
			$name = $selishte->sheets[$i][cells][$j][3];
			$oblast = $selishte->sheets[$i][cells][$j][4];
			$obshstina = $selishte->sheets[$i][cells][$j][5];
			$kmetstvo = $selishte->sheets[$i][cells][$j][6];
			$kind = $selishte->sheets[$i][cells][$j][7];
			$category = $selishte->sheets[$i][cells][$j][8];
			$altitude = $selishte->sheets[$i][cells][$j][9];
			$document = $selishte->sheets[$i][cells][$j][10];
			$tsb = $selishte->sheets[$i][cells][$j][11];
			$abc = $selishte->sheets[$i][cells][$j][12];
			mysql_query("SET NAMES UTF8");
			$query1 = mysql_query("INSERT INTO Selishte(ekatte,t_v_m,name,oblast,obshtina,kmetstvo,kind,category,altitude,document,tsb,abc) values(
				'".$ekatte."','".$tvm."','".$name."','".$oblast."','".$obshstina."','".$kmetstvo."','".$kind."','".$category."','".$altitude."','".$document."','".$tsb."','".$abc."')");
		}
	}
}

?>
