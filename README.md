# easyScreenshot
前言
最近在完善简易QQ项目的过程中，我在基本布局的基础上添加了一些新功能，其中截图功能是首要实现的功能之一。鉴于网上缺乏现成的开源截图软件，我决定自己开发，并在此分享我的源码。初期的实现较为简单，仅涵盖了截图的基本功能。未来若有时间，我会继续对其进行优化和扩展。接下来，直接进入正题。

主要使用的Qt库
QWidget: 作为所有用户界面对象的基础类。
QLabel: 用于显示文本或图像，这里用来展示截取的屏幕快照。
QPushButton: 创建按钮，如确认、取消和保存按钮。
QVBoxLayout 和 QHBoxLayout: 用于布局管理，组织窗口内的小部件。
QMouseEvent: 处理鼠标事件（按下、移动、释放）。
QFileDialog: 提供一个对话框让用户选择文件来保存截图。
QPainter: 用于绘制图形和文本，这里用来绘制截图和选择区域。
QPixmap: 用于表示和操作像素图，存储截图内容。
QRegion: 用于定义和操作不规则形状的区域，用以创建遮罩效果。
QScreen: 获取屏幕信息，并能够抓取整个屏幕的内容。
QColor, QPen, QBrush: 分别用于定义颜色、设置画笔属性和填充形状的颜色或图案，用于绘制选择矩形的边框和阴影区域的颜色。
Qt::Alignment 和其他 Qt::命名空间下的枚举：用于指定对齐方式和其他UI属性。
QRect 和 QPoint: 用于几何计算，比如确定鼠标点击的位置和选择区域的大小。
QtConcurrent::run: 用于异步执行函数，这里是异步保存文件，避免阻塞主线程。
                        
CSDN项目链接：https://blog.csdn.net/LKHzzzzz/article/details/144612842
后续更新：https://blog.csdn.net/LKHzzzzz/article/details/144695517
