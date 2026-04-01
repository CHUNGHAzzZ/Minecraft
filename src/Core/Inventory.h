#pragma once

#include <array>
#include "../World/Block.h"

namespace Minecraft {

struct ItemStack {
    BlockType type = BlockType::Air;
    int count = 0;

    bool IsEmpty() const { return type == BlockType::Air || count <= 0; }
};

class Inventory {
public:
    static constexpr int HOTBAR_SIZE = 9;
    static constexpr int MAX_STACK_SIZE = 64;

    Inventory();

    const ItemStack& GetSlot(int index) const;
    int GetSelectedSlot() const { return m_SelectedSlot; }
    const ItemStack& GetSelectedStack() const { return m_Hotbar[m_SelectedSlot]; }

    void SetSelectedSlot(int index);
    void SelectNextSlot();
    void SelectPrevSlot();

    bool AddItem(BlockType type, int count = 1);

private:
    std::array<ItemStack, HOTBAR_SIZE> m_Hotbar{};
    int m_SelectedSlot = 0;
};

} // namespace Minecraft
