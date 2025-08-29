; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [16 x i8] c"Hello, World!\\n\00", align 1

define i32 @main() {
entry:
  br label %while_cond

while_cond:                                       ; preds = %while_body, %entry
  br i1 true, label %while_body, label %while_end

while_body:                                       ; preds = %while_cond
  %0 = call i32 (ptr, ...) @printf(ptr @0)
  br label %while_cond

while_end:                                        ; preds = %while_cond
  ret i32 0
}

declare i32 @printf(ptr, ...)