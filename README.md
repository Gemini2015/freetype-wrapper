# FreeType-Wrapper

## FreeType封装
使用固定大小的纹理储存需要的字符映像，同时保存每个字符的uv数据。

## 定制

1.  可以自定义固定纹理的尺寸
    
    ```
    FontManager::CreateFont(const char* name, const char* file, float size, uint32* nID, int texSize = 2048);
    ```
    最后一个参数确定了纹理的尺寸。
    
2.  继承`Font`类以实现自定义的纹理操作函数
    关于纹理的操作主要为`CreateTexture(uint32 *texID)`和`UpdateTexture(uint32 texID, Image *img);`。第一个函数创建一个纹理，返回一个可以标识纹理的ID，后一个函数将纹理数据写入对应纹理ID的纹理对象中。
    目前已经给出了OpenGL的实现，对于D3D来说也是大同小异。

## 参考
实现该封装的相关过程和若干问题，可以参考本人的[博客](http://blog.icodeten.com/game/dev/2015/01/31/opengl-text-drawing-note/ "freetype OpenGL")。
