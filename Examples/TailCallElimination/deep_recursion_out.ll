; ModuleID = 'deep_recursion.ll'
source_filename = "deep_recursion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [24 x i8] c"sum(1000000, 0) = %lld\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i64 @sum(i32 noundef %n, i64 noundef %acc) #0 {
entry:
  %retval = alloca i64, align 8
  %n.addr = alloca i32, align 4
  %acc.addr = alloca i64, align 8
  store i32 %n, ptr %n.addr, align 4
  store i64 %acc, ptr %acc.addr, align 8
  br label %tailrecurse

tailrecurse:                                      ; preds = %if.end, %entry
  %0 = load i32, ptr %n.addr, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %tailrecurse
  %1 = load i64, ptr %acc.addr, align 8
  store i64 %1, ptr %retval, align 8
  br label %return

if.end:                                           ; preds = %tailrecurse
  %2 = load i32, ptr %n.addr, align 4
  %sub = sub nsw i32 %2, 1
  %3 = load i64, ptr %acc.addr, align 8
  %4 = load i32, ptr %n.addr, align 4
  %conv = sext i32 %4 to i64
  %add = add nsw i64 %3, %conv
  store i32 %sub, ptr %n.addr, align 4
  store i64 %add, ptr %acc.addr, align 8
  br label %tailrecurse

return:                                           ; preds = %if.then
  %5 = load i64, ptr %retval, align 8
  ret i64 %5
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  %call = call i64 @sum(i32 noundef 1000000, i64 noundef 0)
  %call1 = call i32 (ptr, ...) @printf(ptr noundef @.str, i64 noundef %call)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 16.0.6 (23ubuntu4)"}
