#pragma once

#include <blend2d/blend2d.h>
#include <string_view>

namespace Drafter {

/**
 * @brief Wraps a Blend2D font for text measurement and screen-space rendering.
 *
 * Load once per typeface/size combination and reuse across frames.
 * All methods silently no-op if the font file failed to load.
 */
class Font {
  public:
    Font() = default;

    /**
     * @brief Load a font from a TrueType or OpenType file.
     *
     * @param path Filesystem path to the font file.
     * @param size Pixel size.
     */
    Font(const char *path, float size);

    /** @brief Returns false if the font file could not be opened. */
    bool IsValid() const { return m_valid; }

    /**
     * @brief Returns the font's horizontal ascent in pixels.
     *
     * The ascent is the distance from the baseline to the top of most glyphs.
     */
    float Ascent() const { return m_ascent; }

    /**
     * @brief Returns ascent + descent (the total line height in pixels).
     */
    float LineHeight() const { return m_line_height; }

    /**
     * @brief Measures the advance width of a UTF-8 string.
     *
     * @param text The string to measure.
     * @return Advance width in pixels, or 0 if the font is invalid.
     */
    float MeasureWidth(std::string_view text) const;

    /**
     * @brief Fills UTF-8 text into a Blend2D context at a given baseline position.
     *
     * @param ctx   The active BLContext (transform already set by caller).
     * @param x     Left edge of the text.
     * @param y     Baseline y position.
     * @param text  The string to render.
     * @param color Fill colour.
     */
    void DrawText(BLContext &ctx, float x, float y,
                  std::string_view text, BLRgba32 color) const;

  private:
    BLFontFace m_face;
    BLFont     m_font;
    float      m_ascent      = 0.f;
    float      m_line_height = 0.f;
    bool       m_valid       = false;
};

} // namespace Drafter
