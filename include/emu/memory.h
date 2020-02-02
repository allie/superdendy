#pragma once

#include "common/data.h"

namespace SuperDendy::Emu {
	// 8-bit memory interface
	class Memory8 {
	public:
		virtual ~Memory8() {};

		virtual Byte peek(Word addr) const = 0;
		virtual Byte read(Word addr) = 0;
		virtual void write(Word addr, Byte val) = 0;
		virtual int get_size() = 0;

		virtual void dump(const char* file);
	};

	class SimpleRAM8 : public Memory8 {
	private:
		Byte* data;
		int size;

	public:
		SimpleRAM8(int size);
		SimpleRAM8(Byte* initial_data, int size);
		~SimpleRAM8();

		Byte peek(Word addr) const override;
		virtual Byte read(Word addr) override;
		void write(Word addr, Byte val) override;
		int get_size() override;
	};
}
