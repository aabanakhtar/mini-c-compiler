; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [2 x i8] c"K\00", align 1
@1 = private unnamed_addr constant [12 x i8] c"kowlculator\00", align 1

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0)
  %1 = call i32 (ptr, ...) @printf(ptr @1)
  ret i32 0
}

declare i32 @printf(ptr, ...)