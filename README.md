# BlurApp-Qt

BlurApp Qt
Based on Opencv

There are two classes which can zoom in or zoom out images and cut the image.

头一次写了一个类。ImageWidget是从网上（抱歉！忘了作者和出处。）找到的一个显示图片的继承QtWidget类，有放大、拖动和另存为的功能。我就想给他加个截图的功能，写成了一个selectrect类。也是继承Widget类，作为一个半透明蒙板返回选取矩形的左上坐标和长宽，在ImageWidget中进行截取图片。要实现全功能需要两个类同时使用。应该可以合成一个类
