/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "EraseTool.h"
#include "ImageEditor.h"
#include "Layer.h"
#include <LibGUI/Action.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Painter.h>
#include <LibGfx/Bitmap.h>

namespace PixelPaint {

EraseTool::EraseTool()
{
}

EraseTool::~EraseTool()
{
}

Gfx::Rect EraseTool::build_rect(const Gfx::Point& pos, const Gfx::Rect& widget_rect)
{
    const int base_eraser_size = 10;
    const int eraser_size = (base_eraser_size * m_thickness);
    const int eraser_radius = eraser_size / 2;
    const auto ex = pos.x();
    const auto ey = pos.y();
    return Gfx::Rect(ex - eraser_radius, ey - eraser_radius, eraser_size, eraser_size).intersected(widget_rect);
}

void EraseTool::on_mousedown(Layer& layer, GUI::MouseEvent& event, GUI::MouseEvent&)
{
    if (event.button() != GUI::MouseButton::Left && event.button() != GUI::MouseButton::Right)
        return;
    Gfx::Rect r = build_rect(event.position(), layer.rect());
    GUI::Painter painter(layer.bitmap());
    painter.clear_rect(r, get_color());
    layer.did_modify_bitmap(*m_editor->image());
}

void EraseTool::on_mousemove(Layer& layer, GUI::MouseEvent& event, GUI::MouseEvent&)
{
    if (event.buttons() & GUI::MouseButton::Left || event.buttons() & GUI::MouseButton::Right) {
        Gfx::Rect r = build_rect(event.position(), layer.rect());
        GUI::Painter painter(layer.bitmap());
        painter.clear_rect(r, get_color());
        layer.did_modify_bitmap(*m_editor->image());
    }
}

void EraseTool::on_tool_button_contextmenu(GUI::ContextMenuEvent& event)
{
    if (!m_context_menu) {
        m_context_menu = GUI::Menu::construct();

        auto eraser_color_toggler = GUI::Action::create_checkable("Use secondary color", [&](auto& action) {
            m_use_secondary_color = action.is_checked();
        });
        eraser_color_toggler->set_checked(m_use_secondary_color);

        m_context_menu->add_action(eraser_color_toggler);
        m_context_menu->add_separator();

        m_thickness_actions.set_exclusive(true);
        auto insert_action = [&](int size, bool checked = false) {
            auto action = GUI::Action::create_checkable(String::number(size), [this, size](auto&) {
                m_thickness = size;
            });
            action->set_checked(checked);
            m_thickness_actions.add_action(*action);
            m_context_menu->add_action(move(action));
        };
        insert_action(1, true);
        insert_action(2);
        insert_action(3);
        insert_action(4);
    }

    m_context_menu->popup(event.screen_position());
}

Color EraseTool::get_color() const
{
    if (m_use_secondary_color)
        return m_editor->secondary_color();
    return Color(255, 255, 255, 0);
}

}
