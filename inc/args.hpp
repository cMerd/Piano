
class args {
public:
  bool help;
  bool version;
  bool log;
  bool raylibLogs;

public:
  void parse(int argc, char *argv[]);
};

