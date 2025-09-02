; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [16 x i8] c"Hello, World!\\n\00", align 1
@1 = private unnamed_addr constant [16 x i8] c"Hello, World!\\n\00", align 1

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 3, ptr %i, align 4
  br label %while_cond

while_cond:                                       ; preds = %while_body, %entry
  %loadedVal = load i32, ptr %i, align 4
  %0 = icmp ne i32 %loadedVal, 5
  %1 = sext i1 %0 to i32
  %whilecond = icmp ne i32 %1, 0
  br i1 %whilecond, label %while_body, label %while_end

while_body:                                       ; preds = %while_cond
  %2 = call i32 (ptr, ...) @printf(ptr @0)
  %3 = call i32 (ptr, ...) @printf(ptr @1)
  %loadedVal1 = load i32, ptr %i, align 4
  %4 = add i32 %loadedVal1, 1
  %loadedVal2 = load i32, ptr %i, align 4
  store i32 %4, ptr %i, align 4
  br label %while_cond

while_end:                                        ; preds = %while_cond
  ret i32 0
}

declare i32 @printf(ptr, ...)