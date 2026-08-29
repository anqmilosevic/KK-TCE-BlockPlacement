; ModuleID = 'tail_call.ll'
source_filename = "tail_call.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str.1 = private unnamed_addr constant [23 x i8] c"factorial(10, 1) = %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [18 x i8] c"gcd(48, 18) = %d\0A\00", align 1
@.str.3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str.4 = private unnamed_addr constant [20 x i8] c"fibonacci(10) = %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @factorial(i32 noundef %n, i32 noundef %acc) #0 {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  %acc.addr = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  store i32 %acc, ptr %acc.addr, align 4
  br label %tailrecurse

tailrecurse:                                      ; preds = %if.end, %entry
  %0 = load i32, ptr %n.addr, align 4
  %cmp = icmp sle i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %tailrecurse
  %1 = load i32, ptr %acc.addr, align 4
  store i32 %1, ptr %retval, align 4
  br label %return

if.end:                                           ; preds = %tailrecurse
  %2 = load i32, ptr %n.addr, align 4
  %sub = sub nsw i32 %2, 1
  %3 = load i32, ptr %n.addr, align 4
  %4 = load i32, ptr %acc.addr, align 4
  %mul = mul nsw i32 %3, %4
  store i32 %sub, ptr %n.addr, align 4
  store i32 %mul, ptr %acc.addr, align 4
  br label %tailrecurse

return:                                           ; preds = %if.then
  %5 = load i32, ptr %retval, align 4
  ret i32 %5
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @gcd(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  br label %tailrecurse

tailrecurse:                                      ; preds = %if.end3, %if.then2, %entry
  %0 = load i32, ptr %b.addr, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %tailrecurse
  %1 = load i32, ptr %a.addr, align 4
  store i32 %1, ptr %retval, align 4
  br label %return

if.end:                                           ; preds = %tailrecurse
  %2 = load i32, ptr %a.addr, align 4
  %3 = load i32, ptr %b.addr, align 4
  %cmp1 = icmp sgt i32 %2, %3
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  %4 = load i32, ptr %a.addr, align 4
  %5 = load i32, ptr %b.addr, align 4
  %sub = sub nsw i32 %4, %5
  %6 = load i32, ptr %b.addr, align 4
  store i32 %sub, ptr %a.addr, align 4
  store i32 %6, ptr %b.addr, align 4
  br label %tailrecurse

if.end3:                                          ; preds = %if.end
  %7 = load i32, ptr %a.addr, align 4
  %8 = load i32, ptr %b.addr, align 4
  %9 = load i32, ptr %a.addr, align 4
  %sub4 = sub nsw i32 %8, %9
  store i32 %7, ptr %a.addr, align 4
  store i32 %sub4, ptr %b.addr, align 4
  br label %tailrecurse

return:                                           ; preds = %if.then
  %10 = load i32, ptr %retval, align 4
  ret i32 %10
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @countdown(i32 noundef %n) #0 {
entry:
  %n.addr = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  br label %tailrecurse

tailrecurse:                                      ; preds = %if.end, %entry
  %0 = load i32, ptr %n.addr, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %tailrecurse
  br label %return

if.end:                                           ; preds = %tailrecurse
  %1 = load i32, ptr %n.addr, align 4
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %1)
  %2 = load i32, ptr %n.addr, align 4
  %sub = sub nsw i32 %2, 1
  store i32 %sub, ptr %n.addr, align 4
  br label %tailrecurse

return:                                           ; preds = %if.then
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @fibonacci(i32 noundef %n) #0 {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  %0 = load i32, ptr %n.addr, align 4
  %cmp = icmp slt i32 %0, 2
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %1 = load i32, ptr %n.addr, align 4
  store i32 %1, ptr %retval, align 4
  br label %return

if.end:                                           ; preds = %entry
  %2 = load i32, ptr %n.addr, align 4
  %sub = sub nsw i32 %2, 1
  %call = call i32 @fibonacci(i32 noundef %sub)
  %3 = load i32, ptr %n.addr, align 4
  %sub1 = sub nsw i32 %3, 2
  %call2 = call i32 @fibonacci(i32 noundef %sub1)
  %add = add nsw i32 %call, %call2
  store i32 %add, ptr %retval, align 4
  br label %return

return:                                           ; preds = %if.end, %if.then
  %4 = load i32, ptr %retval, align 4
  ret i32 %4
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  %call = call i32 @factorial(i32 noundef 10, i32 noundef 1)
  %call1 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %call)
  %call2 = call i32 @gcd(i32 noundef 48, i32 noundef 18)
  %call3 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %call2)
  call void @countdown(i32 noundef 5)
  %call4 = call i32 (ptr, ...) @printf(ptr noundef @.str.3)
  %call5 = call i32 @fibonacci(i32 noundef 10)
  %call6 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %call5)
  ret i32 0
}

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
