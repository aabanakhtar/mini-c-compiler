; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [3 x i8] c"hi\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"bye\00", align 1

define i32 @main() {
entry:
  br i1 false, label %if, label %else

if:                                               ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @0)
  br label %merge

else:                                             ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @1)
  br label %merge

merge:                                            ; preds = %else, %if
  %qwerty = call i32 (ptr, ...) @printf(ptr @1)
  ret i32 0
}

declare i32 @printf(ptr, ...)