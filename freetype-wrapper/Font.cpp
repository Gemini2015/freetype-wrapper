#include "Font.h"
#include "ft2build.h"

#ifdef _DEBUG

#include "freetype.h"
#include "fterrors.h"

#else

#include FT_FREETYPE_H
#include FT_ERRORS_H

#endif

#include<gl\glew.h>
#include<gl\GL.h>
#include<gl\GLU.h>

namespace FTWRAPPER
{

	Font::Font(const char* name, const char* file,
		int texSize, float fontSize, uint32 nID)
	{
		m_name.assign(name);
		m_sourceName.assign(file);
		m_size = fontSize;
		m_uuid = nID;
		m_type = FT_NONE;
		m_texSize = texSize;
	}

	Font::~Font()
	{
	}

	void Font::AddCodePointRange(uint32 from, uint32 to)
	{
		if (from >= to)
			return;
		CodePointRange range(from, to);
		m_tempCodePointRangeList.push_back(range);
	}

	uint32 Font::GetTextureID(uint32 codepoint)
	{
		uint32 texID = -1;
		CodePointRange_Vec::iterator it = m_codePointRangeList.begin();
		while (it != m_codePointRangeList.end())
		{
			if (codepoint >= it->from && codepoint <= it->to)
			{
				texID = it->texID;
				break;
			}
			it++;
		}
		return texID;
	}

	Font::Glyph Font::GetGlyph(uint32 texID, uint32 codepoint)
	{
		Glyph glyph;
		SkinGlyph_Map::iterator it = m_texGlyphMap.find(texID);
		if (it != m_texGlyphMap.end())
		{
			Glyph_Map::iterator it_glyph = it->second.find(codepoint);
			if (it_glyph != it->second.end())
			{
				return it_glyph->second;
			}
			else
			{
				return glyph;
			}
		}
		else return glyph;
	}

	FTWRAPPER_enum Font::LoadFont()
	{
		FT_Library library;
		FT_Error error = FT_Init_FreeType(&library);
		if (error)
		{
			return RET_FAIL;
		}

		FT_Face face;
		error = FT_New_Face(library, m_sourceName.c_str(), 0, &face);
		if (error)
		{
			return RET_FAIL;
		}

		error = FT_Set_Pixel_Sizes(face, m_size, 0);
		if (error)
		{
			return RET_FAIL;
		}

		if (m_tempCodePointRangeList.empty())
		{
			CodePointRange range(33, 255);
			m_tempCodePointRangeList.push_back(range);
		}

		// �ж���������
		m_type = face->face_flags & FT_FACE_FLAG_SCALABLE ? FT_TRUETYPE : FT_BITMAP;

		// ��ȡCell��С
		int max_width = 0, max_height = 0;
		int max_bearingY = 0;
		CodePointRange_Vec::iterator it = m_tempCodePointRangeList.begin();
		while (it != m_tempCodePointRangeList.end())
		{
			//����ÿ������
			uint32 from = it->from;
			uint32 to = it->to;
			for (uint32 i = from; i <= to; i++)
			{
				// ����ÿ���ַ�
				error = FT_Load_Char(face, i, FT_LOAD_RENDER);
				if (error)
				{
					continue;
				}

				int height = 2 * (face->glyph->bitmap.rows) - (face->glyph->metrics.horiBearingY >> 6);
				if (height > max_height)
					max_height = height;
				if ((face->glyph->bitmap.pitch) > max_width)
					max_width = (face->glyph->bitmap.pitch);
				if (face->glyph->metrics.horiBearingY > max_bearingY)
					max_bearingY = face->glyph->metrics.horiBearingY;
			}
			it++;
		}
		max_bearingY = max_bearingY >> 6;

		// ����һ�Ź̶���С������
		Image img;
		img.width = m_texSize;
		img.height = m_texSize;
		img.pixelsize = 2;
		img.pitch = img.width * img.pixelsize;
		img.format = Image::IPF_LUMINANCE_ALPHA;
		int image_size = img.pitch * img.height;
		img.data = new unsigned char[image_size];
		memset(img.data, 0, image_size);

		int max_pitch = max_width * img.pixelsize;

		uint32 texID = 0;
		FTWRAPPER_enum ret = CreateTexture(&texID);
		Glyph_Map glyphmap;
		m_texGlyphMap[texID] = glyphmap;

		int penx = 0;
		int peny = 0;
		int rangefrom = 0;
		int rangeto = 0;

		it = m_tempCodePointRangeList.begin();
		while (it != m_tempCodePointRangeList.end())
		{
			//����ÿ������
			uint32 from = it->from;
			uint32 to = it->to;
			rangefrom = from;
			rangeto = from;
			for (uint32 codepoint = from; codepoint <= to; codepoint++, rangeto++)
			{
				// ����ÿ���ַ�
				error = FT_Load_Char(face, codepoint, FT_LOAD_RENDER);
				if (error)
				{
					continue;
				}

				FT_GlyphSlot slot = face->glyph;

				int pitch = slot->bitmap.pitch * img.pixelsize;

				if (penx + max_pitch > img.pitch &&
					peny + 2 * max_height > img.height)
				{
					// ��ǰ�����������
					UpdateTexture(texID, &img);

					CodePointRange range(rangefrom, rangeto - 1);
					range.texID = texID;
					m_codePointRangeList.push_back(range);

					rangefrom = rangeto;

					// ����һ���µ�����
					ret = CreateTexture(&texID);
					memset(img.data, 0, image_size);
					Glyph_Map glyphmap;
					m_texGlyphMap[texID] = glyphmap;

					penx = peny = 0;
				}

				if (penx + pitch > img.pitch)
				{
					// ����
					penx = 0;
					peny += max_height;
				}

				unsigned char* pBuf = (unsigned char*)img.data;
				for (int j = 0; j < slot->bitmap.rows; j++)
				{
					int row = j + peny + max_bearingY - (slot->metrics.horiBearingY >> 6);
					for (int i = 0; i < slot->bitmap.pitch; i++)
					{
						int col = i * img.pixelsize + penx + (slot->metrics.horiBearingX >> 6) * img.pixelsize;
						pBuf[row * img.pitch + col] = slot->bitmap.buffer[j * slot->bitmap.pitch + i];
						pBuf[row * img.pitch + col + 1] = slot->bitmap.buffer[j * slot->bitmap.pitch + i];
					}
				}

				// �˴��漰�� y �᷽�������
				int right = penx + (slot->advance.x >> 6) * img.pixelsize;
				int bottom = peny + max_height;
				Glyph glyph(codepoint,
					(float)penx / img.pitch,
					(float)peny / img.height,
					(float)right / img.pitch,
					(float)bottom / img.height);

				// �������غ��ַ����˴��������
				m_texGlyphMap[texID][codepoint] = glyph;

				penx = penx + max_pitch;

			}

			// �������
			CodePointRange range(rangefrom, rangeto - 1);
			range.texID = texID;
			m_codePointRangeList.push_back(range);

			it++;
		}

		UpdateTexture(texID, &img);

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		m_tempCodePointRangeList.clear();

		if (img.data)
			delete img.data;

		return RET_OK;
	}

	FTWRAPPER_enum Font::CreateTexture(uint32 *texID)
	{
		if (texID == NULL)
			return RET_INVALIDPARAM;

		// ����һ���������
		GLuint texture = 0;
		glGenTextures(1, &texture);

		// ��һ���������
		glBindTexture(GL_TEXTURE_2D, texture);

		// Ϊ��ǰ�󶨵���������������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texSize, m_texSize, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (GLenum error = glGetError())
		{
			if (glIsTexture(texture))
			{
				glDeleteTextures(1, &texture);
			}
			return RET_FAIL;
		}
		*texID = texture;
		return RET_OK;
	}

	FTWRAPPER_enum Font::UpdateTexture(uint32 texID, Image *img)
	{
		if (img == NULL || img->data == NULL)
			return RET_INVALIDPARAM;
		if (!glIsTexture(texID))
			return RET_INVALIDPARAM;

		glBindTexture(GL_TEXTURE_2D, texID);

		// Ϊ��ǰ�󶨵���������������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texSize, m_texSize, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, img->data);
		if (GLenum error = glGetError())
		{
			return RET_FAIL;
		}
		return RET_OK;
	}


	uint32 FontManager::s_UUIDCounter = 0;
	FontManager::FontManager()
	{
	}

	FontManager::~FontManager()
	{
		Font_Map::iterator it = m_fontMap.begin();
		while (it != m_fontMap.end())
		{
			if (it->second)
			{
				delete it->second;
			}
			it++;
		}
	}

	FTWRAPPER_enum FontManager::CreateFont(const char* name, const char* file, float fontSize, uint32* nID, int texSize /*= 2048*/)
	{
		if (name == NULL || strlen(name) == 0 ||
			file == NULL || strlen(file) == 0 ||
			fontSize == 0 || texSize == 0)
			return RET_INVALIDPARAM;

		uint32 id = ++s_UUIDCounter;
		Font* font = new Font(name, file, texSize, fontSize, id);
		if (font != NULL)
		{
			m_fontMap[id] = font;
			if (nID)
			{
				*nID = id;
			}
		}
		return RET_OK;
	}

	Font* FontManager::GetFont(uint32 nFontID)
	{
		Font_Map::iterator it = m_fontMap.find(nFontID);
		if (it != m_fontMap.end())
		{
			return it->second;
		}
		else return NULL;
	}

}