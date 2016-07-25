<?php header('Content-Type: text/html; charset=UTF-8'); ?>
<?php require_once 'db.php';?>

<style>

input{

width: 200px;
height: 40px;
border: 2px solid black;

}
button{

width: 100px;
height: 40px;
background-color: lightblue;
}
div{
display:inline;
}
</style>

<form action="" method="POST">
<div>
<label for="selishte-in">Selishte: </label>
<input name= "selishte-in" type="text"></input>
</div>

<div>
<label for="kmetstvo-in">Kmetstvo: </label>
<input name= "kmetstvo-in" type="text"></input>
</div>

<div>
<label for="Obshtina-in">Obshtina: </label>
<input name= "Obshtina-in" type="text"></input>
</div>

<div>
<label for="Oblast-in">Oblast: </label>
<input name= "Oblast-in" type="text"></input>
</div>

<div>
<label for="Region-in">Region: </label>
<input name= "Region-in" type="text"></input>
<button type="submit"> Show </button>
</div>


</form>


<?php if($_POST["selishte-in"]){
$selishte = $_POST["selishte-in"];
mysql_query("SET NAMES UTF8");
$counter=0;
$get_info = mysql_query("select obsht.name as obshtina, obl.name as oblast, reg.name as region, kmet.name as kmetstvo from Selishte sel inner join Obshtina obsht on sel.obshtina = obsht.obshtina inner join Oblast obl on obsht.oblast = obl.oblast inner join Region reg on obl.region = reg.region inner join Kmetstvo kmet on sel.kmetstvo = kmet.kmetstvo where sel.name='$selishte'");
if($get_info){
echo "Селище: ".$selishte.'<br>';
while($row2= mysql_fetch_array($get_info,MYSQL_ASSOC)){
if($row2['kmetstvo'] == NULL){
$row2['kmetstvo'] = "Неизвестно";
}
echo "Община: ";
echo $row2['obshtina'];
echo "- Област: ";
echo $row2['oblast'];
echo "-Регион: ";
echo $row2['region'];
echo "-Кметство: ";
echo $row2['kmetstvo'];
echo '<br>';
}
}
}?>
<?php if($_POST["kmetstvo-in"]){
$kmetstvo = $_POST["kmetstvo-in"];
mysql_query("SET NAMES UTF8");
$get_info = mysql_query("select obsht.name as obshtina, obl.name as oblast, reg.name as region, sel.name as selishte from Kmetstvo kmet inner join Selishte sel on kmet.kmetstvo = sel.kmetstvo inner join Obshtina obsht on sel.obshtina = obsht.obshtina inner join Oblast obl on obsht.oblast = obl.oblast inner join Region reg on obl.region = reg.region where kmet.name = '$kmetstvo'");
if($get_info){
echo "Кметство: ".$kmetstvo.'<br>';
while($row2= mysql_fetch_array($get_info,MYSQL_ASSOC)){
echo "Селище: ";
echo $row2['selishte'];
echo "Община: ";
echo $row2['obshtina'];
echo "- Област: ";
echo $row2['oblast'];
echo "-Регион: ";
echo $row2['region'];
echo '<br>';

}
}
}?>
<?php if($_POST["Obshtina-in"]){
$obshtina = $_POST["Obshtina-in"];
mysql_query("SET NAMES UTF8");
$counter=0;
$get_info= mysql_query("select distinct ob.name as oblast,reg.name as region from Obshtina obsht inner join Oblast ob on obsht.oblast = ob.oblast inner join Region reg on ob.region = reg.region where obsht.name = '$obshtina'");
if($get_info ){
echo "Община: ".$obshtina."- Област: ";
while($row = mysql_fetch_array($get_info,MYSQL_ASSOC)){
echo $row['oblast'];
echo '- Регион: '.$row['region'];
}
}
$get_info2= mysql_query("select distinct sel.name as selishte, kmet.name as kmetstvo from Obshtina obsht inner join Selishte sel on obsht.obshtina = sel.obshtina inner join Kmetstvo kmet on sel.kmetstvo = kmet.kmetstvo where obsht.name='$obshtina'");
if($get_info2){
echo '<table border="1">';
echo '<tr>';
echo '<td>Номер</td>';
echo '<td>Селище</td>';
echo '<td>Кметство</td>';
echo '</tr>';

while($row2 = mysql_fetch_array($get_info2,MYSQL_ASSOC)){
$counter++;
if($row2['kmetstvo'] == NULL){
$row2['kmetstvo'] = "Неизвестно";
}
echo '<tr>';
echo '<td>'.$counter.'</td>';
echo '<td>'.$row2['selishte'].'</td>';
echo '<td>'.$row2['kmetstvo'].'</td>';
echo '</tr>';
}
echo '</table>';
}
}?>

<?php if($_POST["Oblast-in"]){
$oblast = $_POST["Oblast-in"];
$counter = 0;
mysql_query("SET NAMES UTF8");
$get_info= mysql_query("select ob.name as oblast,reg.name as region from Oblast as ob inner join Region as reg on ob.region = reg.region where ob.name='$oblast'");
if($get_info){
echo "Област: ".$oblast.'- Регион: ';
while($row = mysql_fetch_array($get_info,MYSQL_ASSOC)){
echo $row['region'].' ';
}
}
$get_info2= mysql_query("select obsht.name as obshtini ,sel.name as selishta,kmet.name as kmetstva from Oblast as ob inner join Obshtina as obsht on ob.oblast = obsht.oblast inner join Selishte as sel on obsht.obshtina = sel.obshtina inner join Kmetstvo as kmet on sel.kmetstvo = kmet.kmetstvo where ob.name='$oblast'");
echo '<table border="1">';
echo '<tr>';
echo '<td>Номер</td>';
echo '<td>Общини</td>';
echo '<td>Селища</td>';
echo '<td>Кметства</td>';
echo '</tr>';
if($get_info2){
while($row2 = mysql_fetch_array($get_info2,MYSQL_ASSOC)){
if($row2['kmetstva'] == NULL){
$row2['kmetstva'] = "Неизвестно";
}
$counter++;
echo '<tr>';
echo '<td>'.$counter.'</td>';
echo '<td>'.$row2['obshtini'].'</td>';
echo '<td>'.$row2['selishta'].'</td>';
echo '<td>'.$row2['kmetstva'].'</td>';
echo '</tr>';
}
echo '</table>';
}
}?> 


<?php if($_POST["Region-in"]){
$counter = 0;
$region = $_POST["Region-in"];
mysql_query("SET NAMES UTF8");
$get_info = mysql_query("select sel.name as selishte,kmet.name as kmetstvo,obsht.name as obshtina,obl.name as oblast,reg.name as region from Region reg inner join Oblast obl on reg.region = obl.region inner join Obshtina obsht on obl.oblast = obsht.oblast inner join Selishte sel on obsht.obshtina = sel.obshtina inner join Kmetstvo kmet on sel.kmetstvo=kmet.kmetstvo where reg.name='$region'");
if($get_info){
echo '<table border="1">';
echo '<tr>';
echo '<td>Номер</td>';
echo '<td>Регион</td>';
echo '<td>Област</td>';
echo '<td>Община</td>';
echo '<td>Селище</td>';
echo '<td>Кметство</td>';
echo '</tr>';
while($row = mysql_fetch_array($get_info,MYSQL_ASSOC)){
if($row['kmetstvo'] == NULL){
$row['kmetstvo'] = "Неизвестно";
}
$counter++;
echo '<tr>';
echo '<td>'.$counter.'</td>';
echo '<td>'.$row['region'].'</td>';
echo '<td>'.$row['oblast'].'</td>';
echo '<td>'.$row['obshtina'].'</td>';
echo '<td>'.$row['selishte'].'</td>';
echo '<td>'.$row['kmetstvo'].'</td>';
echo '</tr>';
}
echo '</table>';
}
}?>

