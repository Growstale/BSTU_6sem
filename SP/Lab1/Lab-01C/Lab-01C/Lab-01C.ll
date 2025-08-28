; ModuleID = 'Lab-01C.c'
source_filename = "Lab-01C.c"
target datalayout = "e-m:x-p:32:32-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32-a:0:32-S32"
target triple = "i686-pc-windows-msvc19.43.34808"

$printf = comdat any

$scanf = comdat any

$__local_stdio_printf_options = comdat any

$__local_stdio_scanf_options = comdat any

$"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@" = comdat any

$"??_C@_02DPKJAMEF@?$CFd?$AA@" = comdat any

$"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@" = comdat any

@"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@" = linkonce_odr dso_local unnamed_addr constant [29 x i8] c"Enter a three-digit number: \00", comdat, align 1
@"??_C@_02DPKJAMEF@?$CFd?$AA@" = linkonce_odr dso_local unnamed_addr constant [3 x i8] c"%d\00", comdat, align 1
@"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@" = linkonce_odr dso_local unnamed_addr constant [39 x i8] c"The sum of the digits of a number: %d\0A\00", comdat, align 1
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8
@__local_stdio_scanf_options._OptionsStorage = internal global i64 0, align 8
@str = private unnamed_addr constant [44 x i8] c"Error: a non-three-digit number was entered\00", align 1
@str.1 = private unnamed_addr constant [23 x i8] c"Press Enter to exit...\00", align 1

; Function Attrs: nounwind
define dso_local noundef i32 @main() local_unnamed_addr #0 {
  %1 = alloca i32, align 4
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %1) #8
  %2 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@")
  %3 = call i32 (ptr, ...) @scanf(ptr noundef nonnull @"??_C@_02DPKJAMEF@?$CFd?$AA@", ptr noundef nonnull %1)
  %4 = load i32, ptr %1, align 4
  %5 = add i32 %4, -1000
  %6 = icmp ult i32 %5, -900
  br i1 %6, label %7, label %9

7:                                                ; preds = %0
  %8 = call i32 @puts(ptr nonnull dereferenceable(1) @str)
  br label %18

9:                                                ; preds = %0, %9
  %10 = phi i32 [ %13, %9 ], [ 0, %0 ]
  %11 = phi i32 [ %14, %9 ], [ %4, %0 ]
  %12 = urem i32 %11, 10
  %13 = add nuw nsw i32 %10, %12
  %14 = udiv i32 %11, 10
  %15 = icmp ult i32 %11, 10
  br i1 %15, label %16, label %9, !llvm.loop !4

16:                                               ; preds = %9
  store i32 %14, ptr %1, align 4
  %17 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@", i32 noundef %13)
  br label %18

18:                                               ; preds = %16, %7
  %19 = call i32 @puts(ptr nonnull dereferenceable(1) @str.1)
  %20 = call i32 @getchar()
  %21 = call i32 @getchar()
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %1) #8
  ret i32 0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: inlinehint nounwind
define linkonce_odr dso_local i32 @printf(ptr noundef %0, ...) local_unnamed_addr #2 comdat {
  %2 = alloca ptr, align 4
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %2) #8
  call void @llvm.va_start.p0(ptr nonnull %2)
  %3 = load ptr, ptr %2, align 4
  %4 = call ptr @__acrt_iob_func(i32 noundef 1) #8
  %5 = call ptr @__local_stdio_printf_options()
  %6 = load i64, ptr %5, align 8
  %7 = call i32 @__stdio_common_vfprintf(i64 noundef %6, ptr noundef %4, ptr noundef %0, ptr noundef null, ptr noundef %3) #8
  call void @llvm.va_end.p0(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %2) #8
  ret i32 %7
}

; Function Attrs: inlinehint nounwind
define linkonce_odr dso_local i32 @scanf(ptr noundef %0, ...) local_unnamed_addr #2 comdat {
  %2 = alloca ptr, align 4
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %2) #8
  call void @llvm.va_start.p0(ptr nonnull %2)
  %3 = load ptr, ptr %2, align 4
  %4 = call ptr @__acrt_iob_func(i32 noundef 0) #8
  %5 = call ptr @__local_stdio_scanf_options()
  %6 = load i64, ptr %5, align 8
  %7 = call i32 @__stdio_common_vfscanf(i64 noundef %6, ptr noundef %4, ptr noundef %0, ptr noundef null, ptr noundef %3) #8
  call void @llvm.va_end.p0(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %2) #8
  ret i32 %7
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @getchar() local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start.p0(ptr) #4

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end.p0(ptr) #4

; Function Attrs: noinline nounwind
define linkonce_odr dso_local ptr @__local_stdio_printf_options() local_unnamed_addr #5 comdat {
  ret ptr @__local_stdio_printf_options._OptionsStorage
}

declare dso_local ptr @__acrt_iob_func(i32 noundef) local_unnamed_addr #6

declare dso_local i32 @__stdio_common_vfprintf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #6

declare dso_local i32 @__stdio_common_vfscanf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #6

; Function Attrs: noinline nounwind
define linkonce_odr dso_local ptr @__local_stdio_scanf_options() local_unnamed_addr #5 comdat {
  ret ptr @__local_stdio_scanf_options._OptionsStorage
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #7

attributes #0 = { nounwind "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { inlinehint nounwind "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #5 = { noinline nounwind "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nofree nounwind }
attributes #8 = { nounwind }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 2}
!2 = !{i32 1, !"MaxTLSAlign", i32 65536}
!3 = !{!"clang version 19.1.1"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
