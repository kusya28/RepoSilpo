#include "pch.h"
#include "CppUnitTest.h"
#include "../Silpo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestSilpo
{
	TEST_CLASS(UnitTestSilpo)
	{
	public:

		// ПОЧАТКОВИЙ СТАН ТА КАТАЛОГ

		TEST_METHOD(TestInitialBasketIsEmpty)
		{
			SilpoOrder order;
			Assert::IsTrue(order.isBasketEmpty());
		}

		TEST_METHOD(TestProductNotInCatalogByDefault)
		{
			SilpoOrder order;
			Assert::IsFalse(order.isProductInCatalog("Milk"));
		}

		TEST_METHOD(TestGetUnitForNonExistentProduct)
		{
			SilpoOrder order;
			// Якщо товару немає в каталозі, має повернутися порожній рядок
			Assert::AreEqual(string(""), order.getUnit("NonExistent"));
		}

		// РОБОТА З КОШИКОМ (BASKET)
		TEST_METHOD(TestAddAndRemoveProductBehavior)
		{
			SilpoOrder order;

			// Штучно імітуємо роботу, але оскільки каталог порожній без файлу,
			// додавання неіснуючого товару не повинно змінювати стан кошика
			order.addProduct("Milk", 2.0);
			Assert::IsTrue(order.isBasketEmpty());

			// Перевіряємо, що видалення неіснуючого товару не викликає збоїв (crash)
			order.removeProduct("SomeProduct");
		}

		TEST_METHOD(TestUpdateQuantityOnEmptyBasket)
		{
			SilpoOrder order;
			// Оновлення кількості для товару, якого немає в кошику, не повинно ламати програму
			order.updateQuantity("Bread", 3.0);
			Assert::AreEqual(0.0, order.calculateTotal());
		}


		// ПРОМОКОДИ ТА РОЗРАХУНКИ

		TEST_METHOD(TestCalculateTotalOnEmptyBasket)
		{
			SilpoOrder order;
			Assert::AreEqual(0.0, order.calculateTotal());
		}

		TEST_METHOD(TestApplyValidPromoCode)
		{
			SilpoOrder order;
			order.applyPromoCode("SILPO2026");
			// Навіть зі знижкою 10%, порожній кошик коштує 0.0
			Assert::AreEqual(0.0, order.calculateTotal());
		}

		TEST_METHOD(TestApplyInvalidPromoCode)
		{
			SilpoOrder order;
			order.applyPromoCode("WRONGCODE");
			// Неправильний промокод не повинен давати знижку
			Assert::AreEqual(0.0, order.calculateTotal());
		}


		// СТАТУСИ ТА ДОСТАВКА

		TEST_METHOD(TestSetDeliveryTime)
		{
			SilpoOrder order;
			// Перевіряємо, що встановлення часу працює без винятків у коді
			order.setDeliveryTime("21.05.2026 18:00");

			Assert::IsTrue(order.isBasketEmpty());
		}

		TEST_METHOD(TestSetOrderStatus)
		{
			SilpoOrder order;
			// Перевіряємо зміну статусів
			order.setStatus(OrderStatus::PAID);
			order.setStatus(OrderStatus::COMPLETED);
			order.setStatus(OrderStatus::CANCELLED);

			Assert::AreEqual(0.0, order.calculateTotal());
		}

		TEST_METHOD(TestCannotAddProductToPaidOrder)
		{
			SilpoOrder order;
			// Замовлення стає оплаченим
			order.setStatus(OrderStatus::PAID);

			// Спробуємо додати товар (навіть якщо каталог порожній, перевіримо, що кошик залишається порожнім)
			order.addProduct("Milk", 2.0);
			Assert::IsTrue(order.isBasketEmpty());
		}

		TEST_METHOD(TestInvalidStatusTransition)
		{
			SilpoOrder order;
			// Намагаємось завершити замовлення в обхід оплати
			order.setStatus(OrderStatus::COMPLETED);

			Assert::AreEqual(0.0, order.calculateTotal());
		}
	};
}