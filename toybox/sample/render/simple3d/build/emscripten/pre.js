Module['preRun'] = function () {
    FS.createFolder(
        '/', // �e�t�H���_�̎w��
        'resource', // �t�H���_��
        true, // �ǂݍ��݋���
        true // �������݋���(����̗��false�ł��悳��)
    );
    FS.createFolder(
        '/resource', // �e�t�H���_�̎w��
        'tex', // �t�H���_��
        true, // �ǂݍ��݋���
        true // �������݋���(����̗��false�ł��悳��)
    );
    FS.createPreloadedFile(
        '/resource/tex', // �e�t�H���_�̎w��
        'sample.bmp', // �\�[�X���ł̃t�@�C����
        '/resource/tex/sample.bmp', // http�ŃA�N�Z�X����ۂ�URL���w��
        true, // �ǂݍ��݋���
        false // �������݋���
    );
    FS.createFolder(
        '/resource', // �e�t�H���_�̎w��
        'system', // �t�H���_��
        true, // �ǂݍ��݋���
        true // �������݋���(����̗��false�ł��悳��)
    );
    FS.createFolder(
        '/resource/system', // �e�t�H���_�̎w��
        'font', // �t�H���_��
        true, // �ǂݍ��݋���
        true // �������݋���(����̗��false�ł��悳��)
    );
    FS.createPreloadedFile(
        '/resource/system/font', // �e�t�H���_�̎w��
        'font_ascii.tga', // �\�[�X���ł̃t�@�C����
        '/resource/system/font/font_ascii.tga', // http�ŃA�N�Z�X����ۂ�URL���w��
        true, // �ǂݍ��݋���
        false // �������݋���
    );
};