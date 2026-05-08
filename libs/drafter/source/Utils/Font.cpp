#include "Drafter/Utils/Font.h"

namespace Drafter {

Font::Font(const char *path, float size) {
    if (m_face.create_from_file(path) != BL_SUCCESS) return;
    if (m_font.create_from_face(m_face, size) != BL_SUCCESS) return;
    const BLFontMetrics &m = m_font.metrics();
    m_ascent      = m.ascent;
    m_line_height = m.ascent + m.descent;
    m_valid       = true;
}

float Font::MeasureWidth(std::string_view text) const {
    if (!m_valid || text.empty()) return 0.f;
    BLGlyphBuffer gb;
    gb.set_utf8_text(text.data(), text.size());
    m_font.shape(gb);
    BLTextMetrics tm;
    m_font.get_text_metrics(gb, tm);
    return static_cast<float>(tm.advance.x);
}

void Font::DrawText(BLContext &ctx, float x, float y,
                    std::string_view text, BLRgba32 color) const {
    if (!m_valid || text.empty()) return;
    ctx.fill_utf8_text(BLPoint(x, y), m_font, text.data(), text.size(), color);
}

} // namespace Drafter
