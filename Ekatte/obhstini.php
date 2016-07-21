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
			
			$obsht = mysql_query("SELECT * FROM Obshtina");
				while($row = mysql_fetch_array($obsht,MYSQL_ASSOC)){

					$o = $row['obshtina'];
	
					$oblast = mysql_query("SELECT * FROM Selishte");
						while($row2 = mysql_fetch_array($oblast,MYSQL_ASSOC)){
								$ob = $row2['obshtina'];
								$obl = $row2['oblast'];
							if($o == $ob){
				$query1 = mysql_query("INSERT INTO Obshtina(Obshtina,ekatte,name,category,document,abc,oblast) values('".$Obshtina."','".$ekatte."','".$name."','".$category."','".$document."','".$abc."','".$obl."')");
								
									break;
			}
		}
	
}
		}	
	
	}
}



?>
