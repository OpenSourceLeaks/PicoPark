Module['preRun'] = function () {
    FS.createFolder(
        '/', // 親フォルダの指定
        'resource', // フォルダ名
        true, // 読み込み許可
        true // 書き込み許可(今回の例はfalseでもよさげ)
    );
    FS.createFolder(
        '/resource', // 親フォルダの指定
        'tex', // フォルダ名
        true, // 読み込み許可
        true // 書き込み許可(今回の例はfalseでもよさげ)
    );
    FS.createPreloadedFile(
        '/resource/tex', // 親フォルダの指定
        'sample.bmp', // ソース中でのファイル名
        '/resource/tex/sample.bmp', // httpでアクセスする際のURLを指定
        true, // 読み込み許可
        false // 書き込み許可
    );
    FS.createFolder(
        '/resource', // 親フォルダの指定
        'system', // フォルダ名
        true, // 読み込み許可
        true // 書き込み許可(今回の例はfalseでもよさげ)
    );
    FS.createFolder(
        '/resource/system', // 親フォルダの指定
        'font', // フォルダ名
        true, // 読み込み許可
        true // 書き込み許可(今回の例はfalseでもよさげ)
    );
    FS.createPreloadedFile(
        '/resource/system/font', // 親フォルダの指定
        'font_ascii.tga', // ソース中でのファイル名
        '/resource/system/font/font_ascii.tga', // httpでアクセスする際のURLを指定
        true, // 読み込み許可
        false // 書き込み許可
    );
};