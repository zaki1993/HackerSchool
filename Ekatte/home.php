<style>

#kmetstvo-input{

width: 300px;
height: 50px;
border: 2px solid black;

}
#kmetstvo-button{

width: 100px;
height: 50px;
background-color: lightblue;
}
</style>

<input id="kmetstvo-input" type="text" placeholder="Kmetstvo"></input>
<button id="kmetstvo-button"> Show </button>

<?php header('Content-Type: text/html; charset=UTF-8'); ?>
<?php
require_once 'excel_reader2.php';
require_once 'db.php';


$conn.close();
?>
