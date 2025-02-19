#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <imagine/config/defs.hh>
#include <imagine/gfx/defs.hh>
#include <imagine/gfx/Quads.hh>
#include <imagine/gui/ScrollView.hh>
#include <imagine/util/rectangle2.h>
#include <imagine/util/concepts.hh>
#include <string_view>

namespace IG::Input
{
class Event;
}

namespace IG
{

class Window;
class MenuItem;

class TableView : public ScrollView
{
public:
	using ItemsDelegate = DelegateFunc<size_t (const TableView &view)>;
	using ItemDelegate = DelegateFunc<MenuItem& (const TableView &view, size_t idx)>;
	using SelectElementDelegate = DelegateFunc<void (const Input::Event &, int i, MenuItem &)>;

	TableView(UTF16Convertible auto &&name, ViewAttachParams attach, ItemsDelegate items, ItemDelegate item):
		ScrollView{attach}, items{items}, item{item}, nameStr{IG_forward(name)},
		selectQuads{attach.rendererTask, {.size = 1}},
		separatorQuads{attach.rendererTask, {.size = maxSeparators, .usageHint = Gfx::BufferUsageHint::streaming}} {}

	TableView(ViewAttachParams attach, Container auto &item):
		TableView{UTF16String{}, attach, item} {}

	TableView(UTF16Convertible auto &&name, ViewAttachParams attach, Container auto &item):
		TableView
		{
			IG_forward(name),
			attach,
			[&item](const TableView &) { return std::size(item); },
			[&item](const TableView &, size_t idx) -> MenuItem& { return indirect(std::data(item)[idx]); }
		} {}

	TableView(ViewAttachParams attach, ItemsDelegate items, ItemDelegate item):
		TableView{UTF16String{}, attach, items, item} {}

	void prepareDraw() override;
	void draw(Gfx::RendererCommands &__restrict__) override;
	void place() override;
	void setScrollableIfNeeded(bool yes);
	void scrollToFocusRect();
	void resetScroll();
	bool inputEvent(const Input::Event &) override;
	void clearSelection() override;
	void onShow() override;
	void onHide() override;
	void onAddedToController(ViewController *, const Input::Event &) override;
	void setFocus(bool focused) override;
	void setOnSelectElement(SelectElementDelegate del);
	size_t cells() const;
	WSize cellSize() const;
	void highlightCell(int idx);
	int highlightedCell() const { return selected; }
	[[nodiscard]] TableUIState saveUIState() const;
	void restoreUIState(TableUIState);
	void setAlign(_2DOrigin align);
	std::u16string_view name() const override;
	void resetName(UTF16Convertible auto &&name) { nameStr = IG_forward(name); }
	void resetName() { nameStr.clear(); }
	void setItemsDelegate(ItemsDelegate items_ = [](const TableView &){ return 0; }) { items = items_; }

protected:
	static constexpr size_t maxSeparators = 30;
	ItemsDelegate items{};
	ItemDelegate item{};
	SelectElementDelegate selectElementDel{};
	UTF16String nameStr{};
	Gfx::IQuads selectQuads;
	Gfx::IColQuads separatorQuads;
	int yCellSize = 0;
	int selected = -1;
	int visibleCells = 0;
	_2DOrigin align{LC2DO};
	bool onlyScrollIfNeeded = false;
	bool selectedIsActivated = false;
	bool hasFocus = true;

	void setYCellSize(int s);
	WRect focusRect();
	void onSelectElement(const Input::Event &, size_t i, MenuItem &);
	bool elementIsSelectable(MenuItem &item);
	int nextSelectableElement(int start, int items);
	int prevSelectableElement(int start, int items);
	bool handleTableInput(const Input::Event &, bool &movedSelected);
	virtual void drawElement(Gfx::RendererCommands &__restrict__, size_t i, MenuItem &item, WRect rect, int xIndent) const;
};

}
