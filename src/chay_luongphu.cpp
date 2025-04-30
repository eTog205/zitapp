// chay_luongphu.cpp
#include "chay_luongphu.h"
#include "chucnang_cotloi.h"
#include "csdl.h"
#include "logic_giaodien.h"
#include "thread_pool.h"

void lp_chay_capnhat()
{
	csdl c;
	pool.enqueue(capnhat_data, c);
}

void lp_chay_lenhwinget(const std::string& id)
{
	pool.enqueue(chaylenh, id);
}

void lp_suachua_nhieu(bool dism, bool sfc, bool chk)
{
	pool.enqueue(
			[dism, sfc, chk]
			{
				std::vector<std::string> cmds;
				if (dism)
					cmds.emplace_back("dism /Online /Cleanup-Image /RestoreHealth");
				if (sfc)
					cmds.emplace_back("sfc /scannow");
				if (chk)
					cmds.emplace_back("chkdsk C: /scan");
					//chkdsk(cmds); - lỗi - chưa có kế hoạch sửa
				if (!cmds.empty())
					suachua_nhieu(cmds);
			});
}

void lp_nap_cauhinh()
{
	pool.enqueue(nap_cauhinh);
}

void lp_xuat_cauhinh(const std::string& dd)
{
	pool.enqueue(xuat_cauhinh, dd);
}

void lp_nap_cauhinh_macdinh()
{
	pool.enqueue(ch_macdinh);
}

void lp_chay_saochep_ini()
{
	pool.enqueue(saochep_ini);
}

void lq_khoidong_sql()
{
	khoidong_sql();
}
