# Hello and welcome, dear users of Textractor app!

Here is my humble Textractor plugin, named yWindow. It was inspired by Yomichan chrome extension and Chiitrans app.

## Features

- Rough parsing of japanese text with use of dictionaries (currently supported only yomi dictionaries)
- User can translate a substring by hovering mouse with mouse middle button or keyboard shift modifiers.
- User can add/remove dictionaries.
- User can create his own dictionaries.
- User can temporarily turn on/off dictionary via Settings window. For example, useful for game-specific user dictionaries.

## Known bugs

- yWindow freezes/crashes on exit from Textractor or on delete from Textractor extensions list if dictionaries are stil in processing state (watch `Processing...` status in statusbar). It is because of wierd threads behaviour in Windows DLLs. If you can help me fix it, please, pull request. Solution: Don't close app while it's not in Ready state (see `Ready` status in statusbar). Processing stage don't take so much time. If you still got freeze just end Textractor process from Windows Task Manager.

## Build

I am using vcpkg. Deps: Qt5, Boost, jsoncpp, Gtest(for tests). If you are building x64, make sure you had installed dep with postfix `:x64-windows`.

### x32

```
cmake -Bbin32 -A Win32
cmake --build bin32 --config MinSizeRel -j4
```

### x64

```
cmake -Bbin64 -A x64`
cmake --build bin64 --config MinSizeRel -j4
```

## Data
- All yWindow data is stored in %TEXTRACTOR_ROOT%/yWindow folder.
- All dicts are stored in %TEXTRACTOR_ROOT%/yWindow/dicts folder.
- CSS style is stored in %TEXTRACTOR_ROOT%/yWindow/templates/ytranslation.css. You can change certain css properties as you wish. See [QT Supported CSS Properties](https://doc.qt.io/qt-5/richtext-html-subset.html#css-properties).
- Html template for translation window is stored in %TEXTRACTOR_ROOT%/yWindow/templates/ytranslation.html. _Don't change it if you don't know what are you doing!_

## Managing dictionaries

All changes of dictionaries will be adapted on next translation sencence.
Each dictionary can be deleted (delete folder or file with dictionary) or added (see dictionary-specific instructions below) in runtime.
Each dictionary can be turned on/off temporarily by Options window.
Each dictionary directory or filepath can be open via right-click menu in Options window.

### Yomichan dictionaries

Directory: %TEXTRACTOR_ROOT%/yWindow/dicts/yomi

Yomi dictionary should consist of next files:
- index.json
- tag_bank_1.json
- some of term_bank_*.json or kanji_bank_*.json

Good place to search a dictionary is [yomichan homepage](https://foosoft.net/projects/yomichan/).

Just unzip downloaded file and put in %TEXTRACTOR_ROOT%/yWindow/dicts/yomi directory, see yomi/kanjidic_english for example.

### User dictionaries

Directory: %TEXTRACTOR_ROOT%/yWindow/dicts/user

You can add your own dictionary. Just create %YOUR_DICT%.txt in directory and fill it.

User dictionary should consist of bunch of lines, each has following structure: `word = reading, meaning`. You can comment-out a line by addint `#` into the beginning of it. `, meaning` part is optional.

### Deinflector dictionaries

Directory: %TEXTRACTOR_ROOT%/yWindow/dicts/deinflector

These dictionaries are used internally by other dictionaries and they are very useful. User shouldn't meddle with them.