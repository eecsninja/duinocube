// Copyright (C) 2013 Simon Que
//
// This file is part of DuinoCube.
//
// DuinoCube is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DuinoCube is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DuinoCube.  If not, see <http://www.gnu.org/licenses/>.

// DuinoCube Core shield library for Arduino.

#include "core.h"

#include <Arduino.h>
#include <SPI.h>

#include "file.h"
#include "pins.h"

#define WRITE_BIT_MASK      0x80

extern SPIClass SPI;

namespace DuinoCube {

static File file;

void Core::begin() {
  SET_PIN(CORE_SELECT_DIR, OUTPUT);

  // A rising edge on SS resets the SPI interface logic.
  SET_PIN(CORE_SELECT_PIN, LOW);
  SET_PIN(CORE_SELECT_PIN, HIGH);
}

void Core::moveCamera(int16_t x, int16_t y) {
  // Store these in a contiguous struct so it can be block copied to save time.
  // This works as long as REG_SCROLL_X and REG_SCROLL_Y are contiguous.
  struct {
    int16_t x, y;
  } values;
  values.x = x;
  values.y = y;

  writeData(REG_SCROLL_X, &values, sizeof(values));
}

void Core::waitForEvent(uint16_t events) {
  // Generate two masks. |true_mask| is a mask used to detect if certain bits
  // in a bitfield are set to 1. |false_mask| is used to detect if certain bits
  // are set to 0.
  uint16_t true_mask = 0;
  uint16_t false_mask = 0;

  // TODO: This part could probably be optimized for speed and size with the
  // proper event definitions.
  if (events & CORE_EVENT_VBLANK_BEGIN) {
    true_mask |= (1 << REG_VBLANK);
  }
  if (events & CORE_EVENT_VBLANK_END) {
    false_mask |= (1 << REG_VBLANK);
  }
  if (events & CORE_EVENT_HBLANK_BEGIN) {
    true_mask |= (1 << REG_HBLANK);
  }
  if (events & CORE_EVENT_HBLANK_END) {
    false_mask |= (1 << REG_HBLANK);
  }

  while (true) {
    uint16_t status = readWord(REG_OUTPUT_STATUS);
    if ((true_mask & status) || (false_mask & status)) {
      break;
    }
  }
}

bool Core::loadPalette(const char* filename, uint8_t palette_index) {
  uint16_t handle = file.open(filename, FILE_READ_ONLY);
  if (!handle) {
    return false;
  }
  uint32_t file_size = file.size(handle);
  uint32_t size_read =
      file.readToCore(handle, PALETTE(palette_index), file_size);

  file.close(handle);
  return (file_size == size_read);
}

bool Core::loadTilemap(const char* filename, uint8_t tilemap_index) {
  uint16_t handle = file.open(filename, FILE_READ_ONLY);
  if (!handle) {
    return false;
  }

  writeWord(REG_MEM_BANK, TILEMAP_BANK);

  uint32_t file_size = file.size(handle);
  uint32_t size_read =
      file.readToCore(handle, TILEMAP(tilemap_index), file_size);

  file.close(handle);
  return (file_size == size_read);
}

uint32_t Core::loadImageData(const char* filename, uint32_t vram_offset) {
  uint16_t handle = file.open(filename, FILE_READ_ONLY);
  if (!handle) {
    return false;
  }

  uint32_t file_size = file.size(handle);

  // Allow access to the VRAM.
  writeWord(REG_SYS_CTRL, (1 << REG_SYS_CTRL_VRAM_ACCESS));

  uint32_t total_size_read = 0;
  while (true) {
    // Calculate the bank and offset within the bank.
    uint16_t bank_offset = vram_offset % VRAM_BANK_SIZE;

    // Select the current bank.
    writeWord(REG_MEM_BANK, VRAM_BANK_BEGIN + (vram_offset / VRAM_BANK_SIZE));

    uint16_t bank_size_remaining = VRAM_BANK_SIZE - bank_offset;
    uint16_t size_read =
        file.readToCore(handle, VRAM_BASE + bank_offset, bank_size_remaining);
    // Exit conditions: reached end of data or reached file size.
    if (size_read == 0) {
      break;
    }
    total_size_read += size_read;
    if (total_size_read >= file_size) {
      break;
    }

    // Go to the next bank.
    vram_offset += bank_size_remaining;
  }
  file.close(handle);

  // Turn off access to VRAM.
  writeWord(REG_SYS_CTRL, (0 << REG_SYS_CTRL_VRAM_ACCESS));

  return total_size_read;
}

void Core::writeData(uint16_t addr, const void* data, uint16_t size) {
  SET_PIN(CORE_SELECT_PIN, LOW);

  SPI.transfer(highByte(addr) | WRITE_BIT_MASK);
  SPI.transfer(lowByte(addr));

  const uint8_t* data8 = static_cast<const uint8_t*>(data);
  for (const uint8_t* data_end = data8 + size; data8 < data_end; ++data8)
    SPI.transfer(*data8);

  SET_PIN(CORE_SELECT_PIN, HIGH);
}

void Core::readData(uint16_t addr, void* data, uint16_t size) {
  SET_PIN(CORE_SELECT_PIN, LOW);

  SPI.transfer(highByte(addr) & ~WRITE_BIT_MASK);
  SPI.transfer(lowByte(addr));

  uint8_t* data8 = static_cast<uint8_t*>(data);
  for (uint8_t* data_end = data8 + size; data8 < data_end; ++data8)
    *data8 = SPI.transfer(lowByte(addr));

  SET_PIN(CORE_SELECT_PIN, HIGH);
}

void Core::writeByte(uint16_t addr, uint8_t data) {
  SET_PIN(CORE_SELECT_PIN, LOW);

  SPI.transfer(highByte(addr) | WRITE_BIT_MASK);
  SPI.transfer(lowByte(addr));
  SPI.transfer(data);

  SET_PIN(CORE_SELECT_PIN, HIGH);
}

uint8_t Core::readByte(uint16_t addr) {
  SET_PIN(CORE_SELECT_PIN, LOW);

  SPI.transfer(highByte(addr) & ~WRITE_BIT_MASK);
  SPI.transfer(lowByte(addr));
  uint8_t result = SPI.transfer(0);

  SET_PIN(CORE_SELECT_PIN, HIGH);

  return result;
}

void Core::writeWord(uint16_t addr, uint16_t data) {
  SET_PIN(CORE_SELECT_PIN, LOW);

  SPI.transfer(highByte(addr) | WRITE_BIT_MASK);
  SPI.transfer(lowByte(addr));
  SPI.transfer(lowByte(data));
  SPI.transfer(highByte(data));

  SET_PIN(CORE_SELECT_PIN, HIGH);
}

uint16_t Core::readWord(uint16_t addr) {
  SET_PIN(CORE_SELECT_PIN, LOW);

  SPI.transfer(highByte(addr) & ~WRITE_BIT_MASK);
  SPI.transfer(lowByte(addr));
  union {
    uint16_t value_16;
    uint8_t value_8[2];
  };
  value_8[0] = SPI.transfer(0);  // Low byte.
  value_8[1] = SPI.transfer(0);  // High byte.

  SET_PIN(CORE_SELECT_PIN, HIGH);

  return value_16;
}

}  // namespace DuinoCube
