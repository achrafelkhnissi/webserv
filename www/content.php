<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>content</title>
</head>
<body>
    <?php
        session_start();
        if (!isset($_SESSION["secret"])) {
            header("Location: /login.php", true, 302);
        }
        else {
            echo "<p><b>Secret: </b>" . $_SESSION["secret"] . "</p>";
            echo "<p><b>username: </b>" . $_SESSION["username"] . "</p>";
        }
    ?>
</body>
</html>