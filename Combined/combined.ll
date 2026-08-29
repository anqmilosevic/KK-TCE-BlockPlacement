; ModuleID = 'combined.c'
source_filename = "combined.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [29 x i8] c"Greska: negativan argument!\0A\00", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"factorial(20, 1) = %lld\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i64 @factorial(i32 noundef %n, i64 noundef %acc) #0 {
entry:
  %retval = alloca i64, align 8
  %n.addr = alloca i32, align 4
  %acc.addr = alloca i64, align 8
  store i32 %n, ptr %n.addr, align 4
  store i64 %acc, ptr %acc.addr, align 8
  %0 = load i32, ptr %n.addr, align 4
  %cmp = icmp slt i32 %0, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str)
  call void @exit(i32 noundef 1) #3
  unreachable

if.end:                                           ; preds = %entry
  %1 = load i32, ptr %n.addr, align 4
  %cmp1 = icmp sle i32 %1, 1
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  %2 = load i64, ptr %acc.addr, align 8
  store i64 %2, ptr %retval, align 8
  br label %return

if.end3:                                          ; preds = %if.end
  %3 = load i32, ptr %n.addr, align 4
  %sub = sub nsw i32 %3, 1
  %4 = load i32, ptr %n.addr, align 4
  %conv = sext i32 %4 to i64
  %5 = load i64, ptr %acc.addr, align 8
  %mul = mul nsw i64 %conv, %5
  %call4 = call i64 @factorial(i32 noundef %sub, i64 noundef %mul)
  store i64 %call4, ptr %retval, align 8
  br label %return

return:                                           ; preds = %if.end3, %if.then2
  %6 = load i64, ptr %retval, align 8
  ret i64 %6
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noreturn nounwind
declare void @exit(i32 noundef) #2

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  %call = call i64 @factorial(i32 noundef 20, i64 noundef 1)
  %call1 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i64 noundef %call)
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { noreturn nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 16.0.6 (23ubuntu4)"}
