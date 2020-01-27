#pragma once

#include "common.h"

namespace SuperDendy::Emu {
	class IMemory8 {
	public:
		virtual ~IMemory8() {};

		virtual Byte peek(Word addr) const = 0;
		virtual Byte read(Word addr) = 0;
		virtual void write(Word addr, Byte val) = 0;
		virtual int get_size() = 0;
	};

	class SimpleRAM8 : public IMemory8 {
	private:
		Byte* data;
		int size;

	public:
		SimpleRAM8(int size);
		~SimpleRAM8();

		Byte peek(Word addr) const override;
		virtual Byte read(Word addr) override;
		void write(Word addr, Byte val) override;
		int get_size() override;
	};
}
