# بناء اللعبة على Windows

يمكن تشغيل البناء من GitHub Actions من تبويب **Actions** ثم اختيار **Build Top Speed Arabic Enhanced** والضغط على **Run workflow**. عند أول استخدام قد يطلب GitHub تفعيل Actions للمستودع. بعد نجاح البناء يظهر ملف باسم `TopSpeed-Arabic-Enhanced-Windows` في صفحة التشغيل تحت Artifacts.

Workflow يثبت MSBuild على Windows Runner، لكنه لا يستطيع قانونيًا أو تقنيًا تخمين ملفات DirectX SDK وOGG/Vorbis القديمة غير الموجودة في المستودع. لذلك يجب توفير هذه الاعتمادات في المسارات التي يطلبها README الأصلي:

```text
vs_projects/dxsdk/include
vs_projects/dxsdk/Lib
vs_projects/ogg/include
vs_projects/vorbis/include
```

يمكن البناء محليًا باستخدام Visual Studio Community المجاني أو Visual Studio Build Tools المجاني مع مكوّنات Desktop development with C++. لا يلزم شراء Visual Studio Professional. المشروع قديم ويستخدم Visual Studio 2008 وDirectPlay، لذلك قد يحتاج تحويلًا أو إصلاحات توافق قبل نجاح البناء على Windows الحديث.

عدم نجاح أول تشغيل للـWorkflow لا يعني أن GitHub غير مناسب؛ غالبًا سيكون السبب ملف اعتماد قديمًا مفقودًا أو اختلافًا في إعدادات مشروع Visual Studio القديم. يجب قراءة سجل الخطأ وإصلاح السبب ثم إعادة التشغيل.
