#ifndef _FREETYPE_WRAPPER_FONT_H_
#define _FREETYPE_WRAPPER_FONT_H_

#include <string>
#include <vector>
#include <map>

typedef unsigned long uint32;

namespace FTWRAPPER
{
	enum FTWRAPPER_enum
	{
		//function ret
		RET_OK = 0,
		RET_FAIL,
		RET_INVALIDPARAM,
	};

	class Image
	{
	public:
		enum ImagePixel_Format
		{
			IPF_NONE = 0,
			IPF_RGBA, // R8G8B8A8
			IPF_LUMINANCE,
			IPF_LUMINANCE_ALPHA,
		};

		uint32 width;
		uint32 height;
		uint32 pitch;
		short pixelsize;
		ImagePixel_Format format;
		void* data;

		Image()
		{
			width = 0;
			height = 0;
			pitch = 0;
			pixelsize = 0;
			format = IPF_NONE;
			data = NULL;
		}

		bool operator == (const Image &img) const
		{
			if (this->width == img.width &&
				this->height == img.height &&
				this->pitch == img.pitch &&
				this->pixelsize == img.pixelsize &&
				this->format == img.format &&
				this->data == img.data)
				return true;
			else return false;
		}
	};

	// �������
	class Font
	{
	public:
		enum FontType
		{
			// ȷ����ǰ���������Դ������
			FT_NONE = 0,
			FT_TRUETYPE,
			FT_BITMAP,
		};

		uint32 m_uuid;

		// ��������
		std::string m_name;

		// ����Դ�ļ���
		std::string m_sourceName;

		// ��������
		FontType m_type;

		// �ֺ�(������Ϊ��λ)
		float m_size;

		// �����С
		int m_texSize;

		// ���νṹ
		typedef struct tagGlyph
		{
			uint32 codepoint;
			float left;
			float top;
			float right;
			float bottom;

			tagGlyph()
			{
				codepoint = 0;
				left = top = right = bottom = 0;
			}

			tagGlyph(uint32 codepoint, float left, float top, float right, float bottom)
			{
				this->codepoint = codepoint;
				this->left = left;
				this->top = top;
				this->right = right;
				this->bottom = bottom;
			}

		}Glyph;

		typedef std::map<uint32, Glyph> Glyph_Map;
		typedef std::map<uint32, Glyph_Map> SkinGlyph_Map;

		// ����ӳ�䣬 Texture -> Glyph_Map, CodePoint -> Glyph
		SkinGlyph_Map m_texGlyphMap;

	public:
		Font(const char* name, const char* file, int texSize, float fontSize, uint32 nID);
		virtual ~Font();

		void AddCodePointRange(uint32 from, uint32 to);
		FTWRAPPER_enum LoadFont();
		uint32 GetTextureID(uint32 codepoint);
		Glyph GetGlyph(uint32 texID, uint32 codepoint);
		float GetFontSize()
		{
			return m_size;
		}

	private:

		typedef struct tagCodePointRange
		{
			uint32 from;
			uint32 to;
			uint32 texID;

			tagCodePointRange()
			{
				from = to = 0;
				texID = -1;
			}

			tagCodePointRange(uint32 from, uint32 to)
			{
				this->from = from;
				this->to = to;
			}
		}CodePointRange;

		typedef std::vector<CodePointRange> CodePointRange_Vec;

		// �ڲ������ַ�����
		CodePointRange_Vec m_codePointRangeList;
		// �ݴ��û�������ַ�����
		CodePointRange_Vec m_tempCodePointRangeList;

	protected:
		virtual FTWRAPPER_enum CreateTexture(uint32 *texID);
		virtual FTWRAPPER_enum UpdateTexture(uint32 texID, Image *img);
	};

	class FontManager
	{
	public:
		FontManager();
		~FontManager();

		virtual FTWRAPPER_enum CreateFont(const char* name, const char* file, float size, uint32* nID, int texSize = 2048);
		Font* GetFont(uint32 nFontID);

	public:
		static uint32 s_UUIDCounter;

		typedef std::map<uint32, Font*> Font_Map;
		Font_Map m_fontMap;
	};
}

#endif