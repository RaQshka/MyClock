

const char *HTML_CONTENT_HOME = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" href="data:,">
    <title>Home Page</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background-color: #f0f8ff;
            color: #333;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: flex-start;
            height: 100vh;
        }

        h1 {
            font-size: 2.5em;
            margin-bottom: 15px;
        }

        h2 {
            font-size: 2em;
            margin-bottom: 20px;
        }

        .container {
            display: flex;
            justify-content: space-around;
            width: 80%;
            margin-bottom: 20px;
        }

        .form-container {
            background: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            flex: 1;
            margin: 10px;
        }

        .form-container form {
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .form-container label {
            margin: 5px 0;
            font-weight: bold;
        }

        input[type="number"], input[type="text"] {
            width: 80%;
            padding: 10px;
            margin: 5px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
            font-size: 1em;
            text-align: center;
        }

        input[type="submit"] {
            background-color: #007bff;
            color: #fff;
            border: none;
            padding: 10px 20px;
            margin-top: 10px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 1em;
        }

        input[type="submit"]:hover {
            background-color: #0056b3;
        }

        .notes-container {
            width: 80%;
            margin-top: 20px;
        }

        .notes-container ul {
            list-style: none;
            padding: 0;
        }

        .notes-container li {
            background: #fff;
            margin: 5px 0;
            padding: 10px;
            border-radius: 5px;
            box-shadow: 0 0 5px rgba(0, 0, 0, 0.1);
        }
        .form-name{
            text-align: center;
        }
    </style>
</head>
<body>
    <h1>Мои часы</h1>
    <div class="container">
        <div class="form-container">
            <h2 class="form-name">Настройка будильника</h2>
            <form method="post" action="/">
                <label for="hour">Часы (HH):</label>
                <input type="number" min="0" max="23" id="hour" name="hour" placeholder="HH">
                <label for="min">Минуты (MM):</label>
                <input type="number" min="0" max="59" id="min" name="min" placeholder="MM">
                <label for="sec">Секунды (SS):</label>
                <input type="number" min="0" max="59" id="sec" name="sec" placeholder="SS">
                <input type="submit" name="submit" value="Установить будильник">
            </form>
        </div>
        <div class="form-container">
            <h2 class="form-name">Прокрутка текста</h2>
            <form method="post" action="/">
                <label for="rollingString">Текст для прокрутки:</label>
                <input type="text" maxlength="16" id="rollingString" name="rollingString" placeholder="Введите текст">
                <label for="scrollDelay">Скорость прокрутки (мс):</label>
                <input type="number" min="100" max="5000" id="scrollDelay" name="scrollDelay" value="300" >
                <label for="scrollAll">Прокручивать все по очереди?</label>
                <select name="scrollAll">
                  <!--Supplement an id here instead of using 'name'-->
                  <option value="true">Прокручивать</option>
                  <option value="false">Оставить только текущую запись</option>
                </select>
                <input type="submit" name="submit" value="Прокручивать надпись">
            </form>
        </div>
    </div>
    
    <h2>Текущий будильник: %STRING_STATE%</h2>
    
      <div class="notes-container">
        <h2>Текущие заметки:</h2>
        <ul id="notesList">
            <!-- Заметки будут динамически добавляться здесь -->
        </ul>
    </div>

    <script>
    document.addEventListener('DOMContentLoaded', function() {
    fetch('/getNotes')
        .then(response => response.text())
        .then(data => {
            const notesList = document.getElementById('notesList');
            notesList.innerHTML = data;
        })
        .catch(error => console.error('Error fetching notes:', error));
});
    </script>
</body>
</html>

)=====";