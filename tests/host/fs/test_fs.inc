static void createFile (const char* name, const char* content)
{
    auto f = FSTYPE.open(name, "w");
    REQUIRE(f);
    if (content) {
        f.print(content);
    }
}

static String readFile (const char* name)
{
    auto f = FSTYPE.open(name, "r");
    if (f) {
        return f.readString();
    }
    return String();
}

static std::set<String> listDir (const char* path)
{
    std::set<String> result;
    Dir dir = FSTYPE.openDir(path);
    while (dir.next()) {
        REQUIRE(result.find(dir.fileName()) == std::end(result));
        result.insert(dir.fileName());
    }
    return result;
}

TEST_CASE(TESTPRE "FS can begin",TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
}

TEST_CASE(TESTPRE "FS can't begin with zero size",TESTPAT)
{
    FS_MOCK_DECLARE(0, 8, 512, "");
    REQUIRE_FALSE(FSTYPE.begin());
}

TEST_CASE(TESTPRE "Before begin is called, open will fail",TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE_FALSE(FSTYPE.open("/foo", "w"));
}

TEST_CASE(TESTPRE "FS can create file",TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("/test", "");
    REQUIRE(FSTYPE.exists("/test"));
}

TEST_CASE(TESTPRE "Files can be written and appended to",TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    {
        File f = FSTYPE.open("config1.txt", "w");
        REQUIRE(f);
        f.println("file 1");
    }
    {
        File f = FSTYPE.open("config1.txt", "a");
        REQUIRE(f);
        f.println("file 1 again");
    }
    {
        File f = FSTYPE.open("config1.txt", "r");
        REQUIRE(f);
        char buf[128];
        size_t len = f.read((uint8_t*)buf, sizeof(buf));
        buf[len] = 0;
        REQUIRE(strcmp(buf, "file 1\r\nfile 1 again\r\n") == 0);
    }
}

TEST_CASE(TESTPRE "Files persist after reset", TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("config1.txt", "file 1");

    FS_MOCK_RESET();
    REQUIRE(FSTYPE.begin());
    REQUIRE(readFile("config1.txt") == "file 1");
}


TEST_CASE(TESTPRE "Filesystem is empty after format", TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.format());
    REQUIRE(FSTYPE.begin());
    createFile("/1", "first");
    createFile("/2", "second");
    REQUIRE(FSTYPE.format());
    Dir root = FSTYPE.openDir("/");
    size_t count = 0;
    while (root.next()) {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE(TESTPRE "File names which are too long are rejected", TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    const char* emptyName = "";
    const char* longName_31 = "/234567890123456789012345678901";
    const char* longName_32 = TOOLONGFILENAME;
    REQUIRE_FALSE(FSTYPE.open(emptyName, "w"));
    REQUIRE_FALSE(FSTYPE.open(emptyName, "r"));
    REQUIRE_FALSE(FSTYPE.exists(emptyName));
    REQUIRE_FALSE(FSTYPE.open(longName_32, "w"));
    REQUIRE_FALSE(FSTYPE.open(longName_32, "r"));
    REQUIRE_FALSE(FSTYPE.exists(longName_32));
    REQUIRE(FSTYPE.open(longName_31, "w"));
    REQUIRE(FSTYPE.open(longName_31, "r"));
    REQUIRE(FSTYPE.exists(longName_31));
}

TEST_CASE(TESTPRE "#1685 Duplicate files", "[fs][bugreport]")
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("/config", "some text");
    createFile("/data", "");
    readFile("/config");
    createFile("/data", "more text");
    auto files = listDir("/");
    REQUIRE(files.size() == 2);
}

TEST_CASE(TESTPRE "#1819 Can list all files with openDir(\"\")", "[fs][bugreport]")
{
    FS_MOCK_DECLARE(96, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("/file1", "some text");
    createFile("/file2", "other text");
    createFile("file3", "more text");
    createFile("sorta-dir/file4", "\n");
    auto files = listDir("");
    REQUIRE(files.size() == 4);
}

#ifdef FS_HAS_DIRS

// We silently make subdirectories if they do not exist and silently remove
// them when they're no longer needed, so make sure we can clean up after
// ourselves.  At some point we may drop this and go to normal POSIX mkdir
// behavior and expose the FS::mkdir() method, but for now this works OK.
TEST_CASE(TESTPRE "Removing all files in a subdir removes that subdir", TESTPAT)
{
    FS_MOCK_DECLARE(128, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("/empty", "");
    createFile("/not_empty", "some text");
    createFile("/another", "more text");
    createFile("/subdir/empty", "");
    createFile("/subdir/not_empty", "text again");
    auto files = listDir("/");
    REQUIRE(files.size() == 4);
    files = listDir("/subdir");
    REQUIRE(files.size() == 2);
    // Delete one of subdir, should still exist afterwards
    FSTYPE.remove("subdir/empty");
    files = listDir("/subdir");
    REQUIRE(files.size() == 1);
    FSTYPE.remove("subdir/not_empty");
    files = listDir("/subdir");
    REQUIRE(files.size() == 0);
    files = listDir("/");
    REQUIRE(files.size() == 3);
    REQUIRE(files.find("subdir") == std::end(files));
}

// LittleFS openDir is slightly different than SPIFFS.  In SPIFFS there
// are no directories and "/" is just another character, so "/a/b/c" is a
// file in the root dir whose name is "/a/b/c".  In LittleFS we have full
// directory support, so "/a/b/c" is a file "c" in the "/a/b" dir.
// This means that if you iterate over dirOpen("/") on SPIFFS you get
// a list of every file, including "subdirs".  On LittleFS, you need to
// explicitly open the subdir to see its files.  This behavior is the
// same as POSIX readdir(), and helps isolate subdirs from each other.
// Also note that the returned filenames in the "dir.next()" operator
// will be in that subdir (i.e. if you opendir("/a/b"); f=dir.next();"
// f.name == "c" and not "/a/b/c" as you would see in SPIFFS.
TEST_CASE(TESTPRE "Dir lists all files", TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("/empty", "");
    createFile("/not_empty", "some text");
    createFile("/another", "more text");
    createFile("/subdir/empty", "");
    createFile("/subdir/not_empty", "text again");
    auto files = listDir("/");
    REQUIRE(files.size() == 4);
    bool empty = (files.find("/empty") != std::end(files)) || (files.find("empty") != std::end(files));
    REQUIRE(empty);
    bool not_empty = (files.find("/not_empty") != std::end(files)) || (files.find("not_empty") != std::end(files));
    REQUIRE(not_empty);
    bool another = (files.find("/another") != std::end(files)) ||  (files.find("another") != std::end(files));
    REQUIRE(another);

    files = listDir("/subdir");
    REQUIRE(files.size() == 2);
    bool sub_empty = (files.find("/empty") != std::end(files)) || (files.find("empty") != std::end(files));
    REQUIRE(sub_empty);
    bool sub_not_empty = (files.find("/not_empty") != std::end(files)) || (files.find("not_empty") != std::end(files));
    REQUIRE(sub_not_empty);
}

File FindFileByName(const File f[], int count, const char *name)
{
    for (int i=0; i<count; i++) {
      if (!strcmp(name, f[i].name())) return f[i];
    }
    return f[0];
}

TEST_CASE(TESTPRE "Listfiles.ino example", TESTPAT)
{
    FS_MOCK_DECLARE(128, 8, 512, "");
    REQUIRE(FSTYPE.format());
    REQUIRE(FSTYPE.begin());

    createFile("file1", "hello");
    createFile("file2", "hola");
    createFile("dir1/file3", "nihao");
    createFile("dir2/dir3/file4", "bonjour");

    File root = FSTYPE.open("/", "r");
    // LittleFS and SDFS are not guaranteed to put the names in order of creation, so
    // manually look for them...
    File f[4];
    f[0] = root.openNextFile();
    f[1] = root.openNextFile();
    f[2] = root.openNextFile();
    f[3] = root.openNextFile();
    File file1 = FindFileByName(f, 4, "file1");
    File file2 = FindFileByName(f, 4, "file2");
    File dir1 = FindFileByName(f, 4, "dir1");
    File dir1_file3 = dir1.openNextFile();
    File dir2 = FindFileByName(f, 4, "dir2");
    File dir2_dir3 = dir2.openNextFile();
    File dir2_dir3_file4 = dir2_dir3.openNextFile();

    bool ok;
    ok = root.isDirectory() && !root.isFile() && !strcmp(root.name(), "/");
    REQUIRE(ok);
    ok = !file1.isDirectory() && file1.isFile() && !strcmp(file1.name(), "file1");
    REQUIRE(ok);
    ok = !file2.isDirectory() && file2.isFile() && !strcmp(file2.name(), "file2");
    REQUIRE(ok);
    ok = dir1.isDirectory() && !dir1.isFile() && !strcmp(dir1.name(), "dir1");
    REQUIRE(ok);
    ok = !dir1_file3.isDirectory() && dir1_file3.isFile() && !strcmp(dir1_file3.name(), "file3") &&
         !strcmp(dir1_file3.fullName(), "dir1/file3");
    REQUIRE(ok);
    ok = dir2.isDirectory() && !dir2.isFile() && !strcmp(dir2.name(), "dir2");
    REQUIRE(ok);
    ok = dir2_dir3.isDirectory() && !dir2_dir3.isFile() && !strcmp(dir2_dir3.name(), "dir3");
    REQUIRE(ok);
    ok = !dir2_dir3_file4.isDirectory() && dir2_dir3_file4.isFile() && !strcmp(dir2_dir3_file4.name(), "file4") &&
         !strcmp(dir2_dir3_file4.fullName(), "dir2/dir3/file4");
    REQUIRE(ok);

    REQUIRE(readFile("/file1") == "hello");
    REQUIRE(readFile("file2") == "hola");
    REQUIRE(readFile("dir1/file3") == "nihao");
    REQUIRE(readFile("/dir2/dir3/file4") == "bonjour");
}

#else // !FS_HAS_DIRS

TEST_CASE(TESTPRE "Dir lists all files", TESTPAT)
{
    FS_MOCK_DECLARE(64, 8, 512, "");
    REQUIRE(FSTYPE.begin());
    createFile("/empty", "");
    createFile("/not_empty", "some text");
    createFile("/another", "more text");
    createFile("/subdir/empty", "");
    createFile("/subdir/not_empty", "text again");
    auto files = listDir("/");
    REQUIRE(files.size() == 5);
    bool empty = (files.find("/empty") != std::end(files)) || (files.find("empty") != std::end(files));
    REQUIRE(empty);
    bool not_empty = (files.find("/not_empty") != std::end(files)) || (files.find("not_empty") != std::end(files));
    REQUIRE(not_empty);
    bool another = (files.find("/another") != std::end(files)) ||  (files.find("another") != std::end(files));
    REQUIRE(another);
    bool sub_empty = (files.find("/subdir/empty") != std::end(files)) || (files.find("subdir/empty") != std::end(files));
    REQUIRE(sub_empty);
    bool sub_not_empty = (files.find("/subdir/not_empty") != std::end(files)) || (files.find("subdir/not_empty") != std::end(files));
    REQUIRE(sub_not_empty);
}

#endif
