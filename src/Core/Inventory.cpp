#include "Inventory.h"
#include <algorithm>

namespace Minecraft {

Inventory::Inventory() {
    // Start empty (survival-style). Items will be added via AddItem().
}

const ItemStack& Inventory::GetSlot(int index) const {
    static ItemStack kEmpty;
    if (index < 0 || index >= HOTBAR_SIZE) {
        return kEmpty;
    }
    return m_Hotbar[index];
}

void Inventory::SetSelectedSlot(int index) {
    if (index < 0 || index >= HOTBAR_SIZE) {
        return;
    }
    m_SelectedSlot = index;
}

void Inventory::SelectNextSlot() {
    m_SelectedSlot = (m_SelectedSlot + 1) % HOTBAR_SIZE;
}

void Inventory::SelectPrevSlot() {
    m_SelectedSlot = (m_SelectedSlot - 1 + HOTBAR_SIZE) % HOTBAR_SIZE;
}

bool Inventory::AddItem(BlockType type, int count) {
    if (type == BlockType::Air || count <= 0) {
        return false;
    }

    int remaining = count;

    for (ItemStack& slot : m_Hotbar) {
        if (slot.type != type || slot.count >= MAX_STACK_SIZE) {
            continue;
        }
        int canAdd = std::min(MAX_STACK_SIZE - slot.count, remaining);
        slot.count += canAdd;
        remaining -= canAdd;
        if (remaining == 0) {
            return true;
        }
    }

    for (ItemStack& slot : m_Hotbar) {
        if (!slot.IsEmpty()) {
            continue;
        }
        int canAdd = std::min(MAX_STACK_SIZE, remaining);
        slot.type = type;
        slot.count = canAdd;
        remaining -= canAdd;
        if (remaining == 0) {
            return true;
        }
    }

    return false;
}

} // namespace Minecraft
