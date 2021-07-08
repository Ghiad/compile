#include "Lexer.h"
#include "Parser.h"
#include "BlockTable.h"
#include "Node.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  auto inputFile = "input.pas"; //默认输入文件名
  auto outputFile = "output.o"; //输出文件名
  auto helpInfo = "Usage: ./Compiler 输入文件名 <OPTIONS>\nOptions :\n    -ir 打印LLVM IR中间代码\n    -o 打印源程序生成的可读的汇编代码\n    -help 查看运行提示";
  int printIR = 0;

  if (argc <= 1)
  {
    std::cout << helpInfo << std::endl;
    return 1;
  }

  //选项分析
  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-help"))
    {
      std::cout << helpInfo << std::endl;
      return 0;
    }
    if (i == 1)
    {
      if (argv[i][0] == '-')
      {
        std::cout << "需要输入文件名！" << std::endl;
        std::cout << helpInfo << std::endl;
        return 1;
      }
      else
      {
        inputFile = argv[i];
      }
    }
    else
    {
      if (!strcmp(argv[i], "-ir"))
        printIR = 1;
      else if (!strcmp(argv[i], "-o"));
      else
      {
        std::cout << helpInfo << std::endl;
        return 0;
      }
    }
  }
  Parser parser = Parser(inputFile);
  if (!parser.PROGRAMSTRUCE())
  {
    exit(1);
  }


  FunctionAST *mainFunc = parser.program;
  mainFunc->Codegen();

  if (printIR)
    TheModule->print(errs(), NULL);

  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  auto TargetTriple = sys::getDefaultTargetTriple();
  TheModule->setTargetTriple(TargetTriple);

  std::string Error;
  auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

  // Print an error and exit if we couldn't find the requested target.
  // This generally occurs if we've forgotten to initialise the
  // TargetRegistry or we have a bogus target triple.
  if (!Target)
  {
    errs() << Error;
    return 1;
  }

  auto CPU = "generic";
  auto Features = "";

  TargetOptions opt;
  auto RM = Optional<Reloc::Model>();
  auto TheTargetMachine =
      Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  TheModule->setDataLayout(TheTargetMachine->createDataLayout());

  std::error_code EC;
  raw_fd_ostream dest(outputFile, EC, sys::fs::OF_None);

  if (EC)
  {
    errs() << "Could not open file: " << EC.message();
    return 1;
  }

  legacy::PassManager pass;
  auto FileType = CGFT_ObjectFile;

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
  {
    errs() << "TheTargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*TheModule);
  dest.flush();

  outs() << "Wrote in " << outputFile << "\n";

  return 0;
}
