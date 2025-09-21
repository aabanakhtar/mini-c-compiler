
; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [10 x i8] c"Fooo.....\00", align 1
@1 = private unnamed_addr constant [7 x i8] c"Hello.\00", align 1

define i32 @add(i32 %a, i32 %b, i32 %c) {
entry:
  %a_asalloca = alloca i32, align 4
  store i32 %a, ptr %a_asalloca, align 4
  %b_asalloca = alloca i32, align 4
  store i32 %b, ptr %b_asalloca, align 4
  %c_asalloca = alloca i32, align 4
  store i32 %c, ptr %c_asalloca, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @0)
  %loada = load i32, ptr %a_asalloca, align 4
  %loadb = load i32, ptr %b_asalloca, align 4
  %plustmp = add i32 %loada, %loadb
  %loadc = load i32, ptr %c_asalloca, align 4
  %plustmp1 = add i32 %plustmp, %loadc
  ret i32 %plustmp1
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %q = alloca i32, align 4
  %callresult = call i32 @add(i32 1, i32 2, i32 1)
  store i32 %callresult, ptr %q, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %whilecond

whilecond:                                        ; preds = %whilebody, %entry
  %loadi = load i32, ptr %i, align 4
  %loadq = load i32, ptr %q, align 4
  %lesseqtmp = icmp sle i32 %loadi, %loadq
  %0 = sext i1 %lesseqtmp to i32
  %whilecond1 = icmp ne i32 %0, 0
  br i1 %whilecond1, label %whilebody, label %whileend

whilebody:                                        ; preds = %whilecond
  %1 = call i32 (ptr, ...) @printf(ptr @1)
  %loadi2 = load i32, ptr %i, align 4
  %plustmp = add i32 %loadi2, 1
  %loadi3 = load i32, ptr %i, align 4
  store i32 %plustmp, ptr %i, align 4
  br label %whilecond

whileend:                                         ; preds = %whilecond
  ret i32 0
}